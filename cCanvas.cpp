#include "cCanvas.h"

cCanvas::cCanvas(QQuickItem *pqi) : QQuickPaintedItem(pqi)
    , m_brushSize(1)
    , m_brushColor("#FF00FF00")
    , m_isSymmetry(false)
    , m_nAxes(1)
    , m_bgColor("#202020")
    , m_scale(1.0)
    , m_isMoveMod(false)
    , m_PixelRatio(1)
    , m_isSaveBg(true)
    , m_isDrawAxes(true)
    , m_axesOpacity(64)
    , m_cvsSize(2000)
    , m_offset(0, 0)
    , m_maxNumSavedLines(25)
{
    this->setSmooth(false);
    m_cvs = new QImage(m_cvsSize, m_cvsSize, QImage::Format_ARGB32_Premultiplied);
    m_savedCvs = new QImage(m_cvsSize, m_cvsSize, QImage::Format_ARGB32_Premultiplied);
    setTempCenterPos();
    clear();
}

cCanvas::~cCanvas()
{
    delete m_cvs;
    delete m_savedCvs;
}

void cCanvas::redo()
{
    if(m_deletedLines.length() > 0)
    {
        m_savedLines.push_back(m_deletedLines.pop());
        m_redrawCvs();
    }
}

void cCanvas::undo()
{
    if(m_savedLines.length() > 0)
    {
        m_deletedLines.push(m_savedLines[m_savedLines.length() - 1]);
        m_savedLines.pop_back();
        m_redrawCvs();
    }
}

void cCanvas::m_redrawCvs()
{
    m_cvs->fill(Qt::transparent);
    *m_cvs = m_savedCvs->copy();
    for(auto& line : m_savedLines)
    {
        m_drawLine(line, m_cvs);
    }
    update();
}

void cCanvas::m_drawLine(const LineObj &line, QImage *cvs)
{
    auto saved_m_bufPoint = m_previuosPoint;
    auto saved_m_brushSize = m_brushSize;
    auto saved_m_brushColor = m_brushColor;
    auto saved_m_isSymmetry = m_isSymmetry;
    auto saved_m_nAxes = m_nAxes;

    m_previuosPoint = line.path[0][0];
    m_brushSize = line.size;
    m_brushColor = line.color;
    m_isSymmetry = line.symmetry;
    m_nAxes = line.nAxes;
    for(auto& points : line.path)
    {
        m_drawPoints(points, cvs);
    }

    m_previuosPoint = saved_m_bufPoint;
    m_brushSize = saved_m_brushSize;;
    m_brushColor = saved_m_brushColor;
    m_isSymmetry = saved_m_isSymmetry;
    m_nAxes = saved_m_nAxes;

}

void cCanvas::startLine()
{
    m_deletedLines.clear();
    if(m_savedLines.length() > m_maxNumSavedLines)
    {
        m_drawLine(m_savedLines[0], m_savedCvs);
        m_savedLines.pop_front();
    }
    LineObj line{{}, m_brushSize, m_brushColor, m_nAxes, m_isSymmetry};
    m_savedLines.push_back(line);
}

void cCanvas::continueLine(const QList<QPoint> &points)
{
    m_savedLines[m_savedLines.length() - 1].path.push_back(points);
    m_drawPoints(points, m_cvs);
}

QPointF cCanvas::m_getPolarCoords(QPoint coords)
{
    QVector2D pos(coords.x() - m_cvsSize / 2, coords.y() - m_cvsSize / 2);
    float radius = pos.length();
    if(radius <= 0)
        return QPointF(0, 0);
    float f = atan(static_cast<float>(pos.y()) / pos.x());
    if (pos.x() < 0)
        f += M_PI;
    return QPointF(trunc(radius), f);
}


void cCanvas::m_drawPoints(const QList<QPoint> &points, QImage *cvs)
{
    QPainter painter(cvs);
    painter.setRenderHint(QPainter::Antialiasing, true);
    auto pen = QPen(QColor(m_brushColor));
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setWidth(m_brushSize);
    painter.setPen(pen);

    float angle = M_PI / 180 * (360.0 / m_nAxes);
    for(auto &point : points)
    {
        if(m_nAxes > 1 || m_isSymmetry)
        {
            QPointF posFrom = m_getPolarCoords(m_previuosPoint);
            QPointF posTo = m_getPolarCoords(point);
            for(int axis = 0; axis < m_nAxes; ++axis)
            {
                QPoint start(m_cvsSize/2 + cos(posFrom.y() + (angle * axis)) * posFrom.x(),
                             m_cvsSize/2 + sin(posFrom.y() + (angle * axis)) * posFrom.x());
                QPoint end(m_cvsSize/2 + cos(posTo.y() + (angle * axis)) * posTo.x(),
                           m_cvsSize/2 + sin(posTo.y() + (angle * axis)) * posTo.x());
                painter.drawLine(start, end);
                if(m_isSymmetry)
                {
                    QPoint start(m_cvsSize/2 + cos((M_PI - posFrom.y() + (angle * axis))) * posFrom.x(),
                                 m_cvsSize/2 + sin((M_PI - posFrom.y() + (angle * axis))) * posFrom.x());
                    QPoint end(m_cvsSize/2 + cos((M_PI - posTo.y() + (angle * axis))) * posTo.x(),
                               m_cvsSize/2 + sin((M_PI - posTo.y() + (angle * axis))) * posTo.x());
                    painter.drawLine(start, end);
                }
            }
        }
        else
        {
            painter.drawLine(m_previuosPoint, point);
        }
        m_previuosPoint = point;
    }

    update();
}


Q_INVOKABLE bool cCanvas::save()
{

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    if(m_isSaveBg)
    {
        QImage tmp(m_cvs->size(), m_cvs->format());
        tmp.fill(QColor(m_bgColor));

        QPainter painter(&tmp);
        painter.drawImage(QPoint(0, 0), *m_cvs);
        tmp.save(&buffer, "PNG");
    }
    else
    {
        m_cvs->save(&buffer, "PNG");
    }
    buffer.close();

    QString filename = "PolarPaint-" + QDateTime::currentDateTime().toString("dd-MM-yy-hh-mm-ss-zzz") + ".png";
    QString path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
                   + "/" + filename;

    QFile file(path);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(buffer.buffer());
        file.close();
    }
    else
    {
        return false;
    }
    if(file.error() != QFileDevice::NoError)
        return false;
    return true;

}


void cCanvas::clear()
{
    m_deletedLines.clear();
    m_savedLines.clear();
    m_cvs->fill(Qt::transparent);
    m_savedCvs->fill(Qt::transparent);
    update();
}

void cCanvas::move(const QPoint &path)
{
    m_offset += path;
}


void cCanvas::moveCenter()
{
    m_offset.setX(0);
    m_offset.setY(0);
}

void cCanvas::paint(QPainter *ppainter)
{
    QRect imgRect(
        getRealPosFromLocal(QPoint(0, 0)),
        QSize(m_cvsSize * (m_scale / m_PixelRatio),m_cvsSize * (m_scale / m_PixelRatio))
        );
    QRect borderRect(imgRect.topLeft() - QPoint(2, 2), imgRect.size() + QSize(4, 4));
    ppainter->setPen(QPen(Qt::black, 2));
    ppainter->drawRect(borderRect);
    ppainter->setPen(QPen(Qt::white, 1));
    ppainter->drawRect(borderRect);
    ppainter->drawImage(imgRect,*m_cvs);

    if(m_isDrawAxes && (m_nAxes > 1 || m_isSymmetry))
    {
        ppainter->setRenderHint(QPainter::Antialiasing, true);
        ppainter->setPen(QPen(QColor(255, 255, 255, m_axesOpacity)));
        float angle = M_PI / 180 * (360.0 / m_nAxes);
        QPointF posFrom = m_getPolarCoords(QPoint(m_cvsSize / 2, m_cvsSize / 2));
        QPointF posTo = m_getPolarCoords(QPoint(m_cvsSize / 2, 0));
        QPointF posTo_symmetry = QPointF(posTo.x(), posTo.y() + angle / 2);
        for(int axis = 0; axis < m_nAxes; ++axis)
        {
            QPoint start(m_cvsSize/2 + cos(posFrom.y() + (angle * axis)) * posFrom.x(),
                         m_cvsSize/2 + sin(posFrom.y() + (angle * axis)) * posFrom.x());
            QPoint end(m_cvsSize/2 + cos(posTo.y() + (angle * axis)) * posTo.x(),
                       m_cvsSize/2 + sin(posTo.y() + (angle * axis)) * posTo.x());
            ppainter->drawLine(getRealPosFromLocal(start), getRealPosFromLocal(end));
            if(m_isSymmetry)
            {
                QPoint start(m_cvsSize/2 + cos((M_PI - posFrom.y() + (angle * axis))) * posFrom.x(),
                             m_cvsSize/2 + sin((M_PI - posFrom.y() + (angle * axis))) * posFrom.x());
                QPoint end(m_cvsSize/2 + cos((M_PI - posTo_symmetry.y() + (angle * axis))) * posTo_symmetry.x(),
                           m_cvsSize/2 + sin((M_PI - posTo_symmetry.y() + (angle * axis))) * posTo_symmetry.x());
                ppainter->drawLine(getRealPosFromLocal(start), getRealPosFromLocal(end));
            }
        }
    }
}

QPoint cCanvas::getLocalPosFromReal(const QPoint& realPos)
{
    return QPoint(
        ((realPos.x() - m_offset.x() - (width() / 2.0 - (m_cvsSize / 2.0 * (m_scale / m_PixelRatio)))) / (m_scale / m_PixelRatio)),
        ((realPos.y() - m_offset.y() - (height() / 2.0 - (m_cvsSize / 2.0 * (m_scale / m_PixelRatio)))) / (m_scale / m_PixelRatio))
        );
}


QPoint cCanvas::getRealPosFromLocal(const QPoint& localPos)
{
    return QPoint(
        width() / 2 - (m_cvsSize / 2 * (m_scale / m_PixelRatio)) + m_offset.x() + (localPos.x() * (m_scale / m_PixelRatio)),
        height() / 2 - (m_cvsSize / 2 * (m_scale / m_PixelRatio)) + m_offset.y() + (localPos.y() * (m_scale / m_PixelRatio))
        );
}

void cCanvas::centeringBy(const QPoint& localPos)
{
    QPoint point = getRealPosFromLocal(localPos);
    QPoint center(width() / 2, height() / 2);
    move(center - point);
}

void cCanvas::setTempCenterPos()
{
    m_scalingCenterPos = getLocalPosFromReal(QPoint(width() / 2, height() / 2));
}

void cCanvas::moveScalingCenterTo(const QPoint& localPos, double step)
{
    m_scalingCenterPos += (localPos - m_scalingCenterPos) * step;
}

void cCanvas::changeScaleWithCentering(double scaleChange)
{
    if((scaleChange > 0 && m_scale + scaleChange < 10.0) || (scaleChange < 0 && m_scale + scaleChange > 0.05))
    {
        m_scale += scaleChange;
        emit scaleChanged(m_scale);
        centeringBy(m_scalingCenterPos);
    }
}

bool cCanvas::symmetry()
{
    return m_isSymmetry;
}
void cCanvas::setSymmetry(bool symmetry)
{
    m_isSymmetry = symmetry;
    update();
}

int cCanvas::axes()
{
    return m_nAxes;
}
void cCanvas::setAxes(int nAxes)
{
    m_nAxes = nAxes;
    update();
}

bool cCanvas::isDrawAxes()
{
    return m_isDrawAxes;
}
void cCanvas::setDrawAxes(bool draw)
{
    m_isDrawAxes = draw;
    update();
}

int cCanvas::axesOpacity()
{
    return m_axesOpacity;
}
void cCanvas::setAxesOpacity(int opacity)
{
    m_axesOpacity = opacity;
    update();
}

int cCanvas::cvsSize()
{
    return m_cvsSize;
}
void cCanvas::setCvsSize(int size)
{
    m_cvsSize = (size > 1?size:2);
    *m_cvs = m_cvs->scaled(m_cvsSize, m_cvsSize);
    *m_savedCvs = m_savedCvs->scaled(m_cvsSize, m_cvsSize);
    setTempCenterPos();
    m_redrawCvs();
}
