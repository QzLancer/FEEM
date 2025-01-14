#include "pf_plot.h"
#include "pf_graphicview.h"

#include <QPainter>
#include <QtMath>
#include <QMouseEvent>
#include <QDebug>
#include <QGridLayout>

#include "pf_actioninterface.h"
#include "pf_eventhandler.h"


/*! \var QCPAxis *PF_GraphicView::xAxis

  A pointer to the primary x Axis (bottom) of the main axis rect of the plot.

  PF_GraphicView offers convenient pointers to the axes (\ref xAxis, \ref yAxis, \ref xAxis2, \ref
  yAxis2) and the \ref legend. They make it very easy working with plots that only have a single
  axis rect and at most one axis at each axis rect side. If you use \link thelayoutsystem the
  layout system\endlink to add multiple axis rects or multiple axes to one side, use the \ref
  QCPAxisRect::axis interface to access the new axes. If one of the four default axes or the
  default legend is removed due to manipulation of the layout system (e.g. by removing the main
  axis rect), the corresponding pointers become 0.

  If an axis convenience pointer is currently zero and a new axis rect or a corresponding axis is
  added in the place of the main axis rect, PF_GraphicView resets the convenience pointers to the
  according new axes. Similarly the \ref legend convenience pointer will be reset if a legend is
  added after the main legend was removed before.
*/

/*! \var QCPAxis *PF_GraphicView::yAxis

  A pointer to the primary y Axis (left) of the main axis rect of the plot.

  PF_GraphicView offers convenient pointers to the axes (\ref xAxis, \ref yAxis, \ref xAxis2, \ref
  yAxis2) and the \ref legend. They make it very easy working with plots that only have a single
  axis rect and at most one axis at each axis rect side. If you use \link thelayoutsystem the
  layout system\endlink to add multiple axis rects or multiple axes to one side, use the \ref
  QCPAxisRect::axis interface to access the new axes. If one of the four default axes or the
  default legend is removed due to manipulation of the layout system (e.g. by removing the main
  axis rect), the corresponding pointers become 0.

  If an axis convenience pointer is currently zero and a new axis rect or a corresponding axis is
  added in the place of the main axis rect, PF_GraphicView resets the convenience pointers to the
  according new axes. Similarly the \ref legend convenience pointer will be reset if a legend is
  added after the main legend was removed before.
*/

/*! \var QCPAxis *PF_GraphicView::xAxis2

  A pointer to the secondary x Axis (top) of the main axis rect of the plot. Secondary axes are
  invisible by default. Use QCPAxis::setVisible to change this (or use \ref
  QCPAxisRect::setupFullAxesBox).

  PF_GraphicView offers convenient pointers to the axes (\ref xAxis, \ref yAxis, \ref xAxis2, \ref
  yAxis2) and the \ref legend. They make it very easy working with plots that only have a single
  axis rect and at most one axis at each axis rect side. If you use \link thelayoutsystem the
  layout system\endlink to add multiple axis rects or multiple axes to one side, use the \ref
  QCPAxisRect::axis interface to access the new axes. If one of the four default axes or the
  default legend is removed due to manipulation of the layout system (e.g. by removing the main
  axis rect), the corresponding pointers become 0.

  If an axis convenience pointer is currently zero and a new axis rect or a corresponding axis is
  added in the place of the main axis rect, PF_GraphicView resets the convenience pointers to the
  according new axes. Similarly the \ref legend convenience pointer will be reset if a legend is
  added after the main legend was removed before.
*/

/*! \var QCPAxis *PF_GraphicView::yAxis2

  A pointer to the secondary y Axis (right) of the main axis rect of the plot. Secondary axes are
  invisible by default. Use QCPAxis::setVisible to change this (or use \ref
  QCPAxisRect::setupFullAxesBox).

  PF_GraphicView offers convenient pointers to the axes (\ref xAxis, \ref yAxis, \ref xAxis2, \ref
  yAxis2) and the \ref legend. They make it very easy working with plots that only have a single
  axis rect and at most one axis at each axis rect side. If you use \link thelayoutsystem the
  layout system\endlink to add multiple axis rects or multiple axes to one side, use the \ref
  QCPAxisRect::axis interface to access the new axes. If one of the four default axes or the
  default legend is removed due to manipulation of the layout system (e.g. by removing the main
  axis rect), the corresponding pointers become 0.

  If an axis convenience pointer is currently zero and a new axis rect or a corresponding axis is
  added in the place of the main axis rect, PF_GraphicView resets the convenience pointers to the
  according new axes. Similarly the \ref legend convenience pointer will be reset if a legend is
  added after the main legend was removed before.
*/

/*! \var QCPLegend *PF_GraphicView::legend

  A pointer to the default legend of the main axis rect. The legend is invisible by default. Use
  QCPLegend::setVisible to change this.

  PF_GraphicView offers convenient pointers to the axes (\ref xAxis, \ref yAxis, \ref xAxis2, \ref
  yAxis2) and the \ref legend. They make it very easy working with plots that only have a single
  axis rect and at most one axis at each axis rect side. If you use \link thelayoutsystem the
  layout system\endlink to add multiple legends to the plot, use the layout system interface to
  access the new legend. For example, legends can be placed inside an axis rect's \ref
  QCPAxisRect::insetLayout "inset layout", and must then also be accessed via the inset layout. If
  the default legend is removed due to manipulation of the layout system (e.g. by removing the main
  axis rect), the corresponding pointer becomes 0.

  If an axis convenience pointer is currently zero and a new axis rect or a corresponding axis is
  added in the place of the main axis rect, PF_GraphicView resets the convenience pointers to the
  according new axes. Similarly the \ref legend convenience pointer will be reset if a legend is
  added after the main legend was removed before.
*/

/* end of documentation of public members */

/*!
  Constructs a PF_GraphicView and sets reasonable default values.
*/
PF_GraphicView::PF_GraphicView(PF_Document *doc, QWidget *parent)
    : QWidget(parent),
    xAxis(nullptr),
    yAxis(nullptr),
    xAxis2(nullptr),
    yAxis2(nullptr),
    legend(nullptr),
    eventHandler(new PF_EventHandler(this)),
    mBufferDevicePixelRatio(1.0), // will be adapted to primary screen below
    mPlotLayout(nullptr),
    mAutoAddPlottableToLegend(true),
    mAntialiasedElements(QCP::aeNone),
    mNotAntialiasedElements(QCP::aeNone),
    mInteractions(nullptr),
    mSelectionTolerance(8),
    mNoAntialiasingOnDrag(false),
    mBackgroundBrush(Qt::white, Qt::SolidPattern),
    mBackgroundScaled(true),
    mBackgroundScaledMode(Qt::KeepAspectRatioByExpanding),
    mCurrentLayer(nullptr),
    mPlottingHints(QCP::phCacheLabels|QCP::phImmediateRefresh),
    mMultiSelectModifier(Qt::ControlModifier),
    mSelectionRectMode(QCP::srmNone),
    mSelectionRect(nullptr),
    mOpenGl(false),
    mMouseHasMoved(false),
    mMouseEventLayerable(nullptr),
    mMouseSignalLayerable(nullptr),
    mReplotting(false),
    mReplotQueued(false),
    mOpenGlMultisamples(16),
    mOpenGlAntialiasedElementsBackup(QCP::aeNone),
    mOpenGlCacheLabelsBackup(true)
{
    //qDebug()<<"PF_GraphicView::PF_GraphicView";
    if(doc){
        setContainer(doc);
        container->mParentPlot = this;
    }

    /**鼠标跟踪失效（默认），当鼠标被移动的时候只有在至少一个鼠标按键被按下时，
    这个窗口部件才会接收鼠标移动事件。**/
    setMouseTracking(true);

    setDefaultSnapMode(PF_SnapMode());


    setAttribute(Qt::WA_NoMousePropagation);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setFocusPolicy(Qt::ClickFocus);
    setMouseTracking(true);
    QLocale currentLocale = locale();
    currentLocale.setNumberOptions(QLocale::OmitGroupSeparator);
    setLocale(currentLocale);
#ifdef QCP_DEVICEPIXELRATIO_SUPPORTED
#  ifdef QCP_DEVICEPIXELRATIO_FLOAT
    setBufferDevicePixelRatio(QWidget::devicePixelRatioF());
#  else
    setBufferDevicePixelRatio(QWidget::devicePixelRatio());
#  endif
#endif

    mOpenGlAntialiasedElementsBackup = mAntialiasedElements;
    mOpenGlCacheLabelsBackup = mPlottingHints.testFlag(QCP::phCacheLabels);
    // create initial layers:
    mLayers.append(new QCPLayer(this, QLatin1String("background")));
    mLayers.append(new QCPLayer(this, QLatin1String("grid")));
    mLayers.append(new QCPLayer(this, QLatin1String("main")));
    mLayers.append(new QCPLayer(this, QLatin1String("axes")));
    mLayers.append(new QCPLayer(this, QLatin1String("legend")));
    mLayers.append(new QCPLayer(this, QLatin1String("overlay")));
    updateLayerIndices();
    setCurrentLayer(QLatin1String("main"));
    layer(QLatin1String("overlay"))->setMode(QCPLayer::lmBuffered);

    // create initial layout, axis rect and legend:
    mPlotLayout = new QCPLayoutGrid;
    mPlotLayout->initializeParentPlot(this);
    mPlotLayout->setParent(this); // important because if parent is QWidget, QCPLayout::sizeConstraintsChanged will call QWidget::updateGeometry
    mPlotLayout->setLayer(QLatin1String("main"));
    QCPAxisRect *defaultAxisRect = new QCPAxisRect(this, true);
    mPlotLayout->addElement(0, 0, defaultAxisRect);
    xAxis = defaultAxisRect->axis(QCPAxis::atBottom);
    yAxis = defaultAxisRect->axis(QCPAxis::atLeft);
    xAxis2 = defaultAxisRect->axis(QCPAxis::atTop);
    yAxis2 = defaultAxisRect->axis(QCPAxis::atRight);
    legend = new QCPLegend;
    legend->setVisible(false);
    defaultAxisRect->insetLayout()->addElement(legend, Qt::AlignRight|Qt::AlignTop);
    defaultAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));

    defaultAxisRect->setLayer(QLatin1String("background"));
    xAxis->setLayer(QLatin1String("axes"));
    yAxis->setLayer(QLatin1String("axes"));
    xAxis2->setLayer(QLatin1String("axes"));
    xAxis2->setVisible(true);
    xAxis2->setTickLabels(false);
    yAxis2->setLayer(QLatin1String("axes"));
    yAxis2->setVisible(true);
    yAxis2->setTickLabels(false);
    xAxis->grid()->setLayer(QLatin1String("grid"));
    xAxis->grid()->setSubGridVisible(true);
    xAxis->grid()->setPen(QPen(QColor(205,220,231)));
    xAxis->grid()->setSubGridPen(QPen(QColor(205,220,231)));
    yAxis->grid()->setLayer(QLatin1String("grid"));
    yAxis->grid()->setSubGridVisible(true);
    yAxis->grid()->setPen(QPen(QColor(205,220,231)));
    yAxis->grid()->setSubGridPen(QPen(QColor(205,220,231)));
    xAxis2->grid()->setLayer(QLatin1String("grid"));
    yAxis2->grid()->setLayer(QLatin1String("grid"));
    legend->setLayer(QLatin1String("legend"));
    /**将container添加到main层，这样才能够进行绘制**/
    container->setLayer(QLatin1String("main"));
    /**设置坐标轴背景**/
    defaultAxisRect->setBackground(QBrush(QColor(248,253,255)));
    /**设置xy轴等比例**/
    //xAxis->setScaleRatio(yAxis,1);/**该函数在构造函数内无效**/
    /**设置top轴和right轴随着其他两个轴变化**/
    connect(defaultAxisRect->axis(QCPAxis::atLeft), SIGNAL(rangeChanged(QCPRange)), defaultAxisRect->axis(QCPAxis::atRight), SLOT(setRange(QCPRange)));
    connect(defaultAxisRect->axis(QCPAxis::atRight), SIGNAL(rangeChanged(QCPRange)), defaultAxisRect->axis(QCPAxis::atLeft), SLOT(setRange(QCPRange)));
    connect(defaultAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), defaultAxisRect->axis(QCPAxis::atTop), SLOT(setRange(QCPRange)));
    connect(defaultAxisRect->axis(QCPAxis::atTop), SIGNAL(rangeChanged(QCPRange)), defaultAxisRect->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    /**设置坐标轴可缩放**/
    setInteractions( QCP::iRangeZoom);
    /**设置坐标轴上tick的长度长一点**/
    int subTickLength = 7;
    int tickLength = 10;
    xAxis->setTickLength(tickLength,0);
    xAxis->setSubTickLength(subTickLength,0);
    xAxis->setSubTickPen(QPen(QColor(128,128,128)));
    yAxis->setTickLength(tickLength,0);
    yAxis->setSubTickLength(subTickLength,0);
    yAxis->setSubTickPen(QPen(QColor(128,128,128)));
    xAxis2->setTickLength(tickLength,0);
    xAxis2->setSubTickLength(subTickLength,0);
    xAxis2->setSubTickPen(QPen(QColor(128,128,128)));
    yAxis2->setTickLength(tickLength,0);
    yAxis2->setSubTickLength(subTickLength,0);
    yAxis2->setSubTickPen(QPen(QColor(128,128,128)));
    /**设置tickLabel字体**/
    xAxis->setTickLabelFont(QFont(QLatin1String("sans serif"), 10));
    yAxis->setTickLabelFont(QFont(QLatin1String("sans serif"), 10));
    // create selection rect instance:
    mSelectionRect = new QCPSelectionRect(this);
    mSelectionRect->setLayer(QLatin1String("overlay"));

    setViewport(rect()); // needs to be called after mPlotLayout has been created

    replot(rpQueuedReplot);
}

PF_GraphicView::~PF_GraphicView()
{
    //clearPlottables();
    clearItems();

    if (mPlotLayout)
    {
        delete mPlotLayout;
        mPlotLayout = nullptr;
    }

    mCurrentLayer = nullptr;
    qDeleteAll(mLayers); // don't use removeLayer, because it would prevent the last layer to be removed
    mLayers.clear();
}

//void PF_GraphicView::paintEvent(QPaintEvent *e){
//    getPixmapForView(&PixmapLayer1);
//    getPixmapForView(&PixmapLayer2);
//    getPixmapForView(&PixmapLayer3);
//    PixmapLayer1->fill(QColor(120,250,250,80));

//    //draw grid
//    QPainter painter1(PixmapLayer1);
//    painter1.setPen(QColor(0,0,0));
//    drawLayer1(&painter1);
//    painter1.end();

//    PixmapLayer2->fill(Qt::transparent);
//    QPainter painter2(PixmapLayer2);
//    painter2.setRenderHint(QPainter::HighQualityAntialiasing, true);
//    QPen pen1 = QPen(QColor(0,0,0));
//    pen1.setWidth(2);
//    painter2.setPen(pen1);

//    drawEntityLayer(&painter2);
//    painter2.end();

//    PixmapLayer3->fill(Qt::transparent);
//    if (redrawMethod & PF::RedrawOverlay)
//    {
//        qDebug()<<"PF_GraphicView::paintEvent RedrawOverlay";

//        QPainter painter3(PixmapLayer3);
//        painter3.setRenderHint(QPainter::HighQualityAntialiasing,true);
//        QPen pen1 = QPen(QColor(0,0,0));
//        pen1.setWidth(2);
//        painter3.setPen(pen1);
//        drawLayer3(&painter3);
//        painter3.end();
//    }

//    QPainter painter(this);
//    painter.drawPixmap(0,0,*PixmapLayer1);
//    painter.drawPixmap(0,0,*PixmapLayer2);
//    painter.drawPixmap(0,0,*PixmapLayer3);
//    painter.end();

//    redrawMethod=PF::RedrawNone;
//}


void PF_GraphicView::tabletEvent(QTabletEvent *e)
{
    Q_UNUSED(e)
}

void PF_GraphicView::leaveEvent(QEvent *)
{

}

void PF_GraphicView::enterEvent(QEvent *)
{

}

void PF_GraphicView::focusInEvent(QFocusEvent *)
{

}

void PF_GraphicView::focusOutEvent(QFocusEvent *)
{

}


void PF_GraphicView::keyPressEvent(QKeyEvent *e)
{
    Q_UNUSED(e)
}

void PF_GraphicView::keyReleaseEvent(QKeyEvent *e)
{
    Q_UNUSED(e)
}


PF_EntityContainer *PF_GraphicView::getOverlayContainer(PF::OverlayGraphics position)
{
    if (overlayEntities[position]) {
        return overlayEntities[position];
    }
    overlayEntities[position]=new PF_EntityContainer(nullptr);

    return overlayEntities[position];
}

/**绘制坐标轴和网格**/
//void PF_GraphicView::drawLayer1(QPainter * painter){
//    int numgridw = 10;
//    int numgridh = 5;
//    int leftmargin = 50;
//    int rightmargin = 50;
//    int topmargin = 50;
//    int bottommargin = 50;
//    int spaceW = (width()-rightmargin-leftmargin)/numgridw;
//    int spaceH = (height()-topmargin-bottommargin)/numgridh;
//    int subspaceW = spaceW/5;
//    int subspaceH = spaceH/5;

//    //    for(int i = 0+leftmargin; i < this->width()-rightmargin;i+=10){
//    //        for(int j = 0+topmargin; j < this->height()-bottommargin;j+=10){
//    //            painter->drawPoint(i,j);
//    //        }
//    //    }
//    painter->setPen(Qt::gray);

//    for(int i = 1; i < numgridw;i+=1){
//        painter->drawLine(QPointF(i*spaceW+leftmargin,0+topmargin),QPointF(i*spaceW+leftmargin,spaceH*numgridh+topmargin));
//    }
//    for(int i = 1; i < numgridh;i+=1){
//        painter->drawLine(QPointF(0+leftmargin,i*spaceH+topmargin),QPointF(spaceW*numgridw+leftmargin,i*spaceH+topmargin));
//    }
//    QPen pen1;
//    pen1.setColor(Qt::black);
//    pen1.setWidth(2);
//    painter->setPen(pen1);
//    painter->drawRect(leftmargin,topmargin,spaceW*numgridw,spaceH*numgridh);
//    int mainTickH = 10;
//    int subTickH = 5;
//    for(int i = 0; i < numgridw;i+=1){
//        pen1.setWidth(2);painter->setPen(pen1);
//        painter->drawLine(QPointF(i*spaceW+leftmargin,0+topmargin),QPointF(i*spaceW+leftmargin,mainTickH+topmargin));
//        painter->drawLine(QPointF(i*spaceW+leftmargin,spaceH*numgridh+topmargin),QPointF(i*spaceW+leftmargin,spaceH*numgridh-mainTickH+topmargin));
//        pen1.setWidth(1);painter->setPen(pen1);
//        for(int j = 1; j <= 4;j++){
//            painter->drawLine(QPointF(i*spaceW+leftmargin+j*subspaceW,0+topmargin),QPointF(i*spaceW+leftmargin+j*subspaceW,subTickH+topmargin));
//            painter->drawLine(QPointF(i*spaceW+leftmargin+j*subspaceW,spaceH*numgridh+topmargin),QPointF(i*spaceW+leftmargin+j*subspaceW,spaceH*numgridh-subTickH+topmargin));
//        }
//    }
//    for(int i = 0; i < numgridh;i+=1){
//        pen1.setWidth(2);painter->setPen(pen1);
//        painter->drawLine(QPointF(0+leftmargin,i*spaceH+topmargin),QPointF(mainTickH+leftmargin,i*spaceH+topmargin));
//        painter->drawLine(QPointF(spaceW*numgridw+leftmargin,i*spaceH+topmargin),QPointF(spaceW*numgridw-mainTickH+leftmargin,i*spaceH+topmargin));
//        pen1.setWidth(1);painter->setPen(pen1);
//        for(int j = 1; j <= 4;j++){
//            painter->drawLine(QPointF(0+leftmargin,i*spaceH+topmargin+j*subspaceH),QPointF(subTickH+leftmargin,i*spaceH+topmargin+j*subspaceH));
//            painter->drawLine(QPointF(spaceW*numgridw+leftmargin,i*spaceH+topmargin+j*subspaceH),QPointF(spaceW*numgridw-subTickH+leftmargin,i*spaceH+topmargin+j*subspaceH));
//        }
//    }
//}

//void PF_GraphicView::getPixmapForView(QPixmap **pm)
//{
//    QSize const s0(width(), height());
//    if(*pm){
//        if((*pm)->size()==s0){
//            return;
//        }else{
//            delete *pm;
//            *pm = nullptr;
//        }
//    }

//    *pm = new QPixmap(width(), height());
//}

void PF_GraphicView::back()
{
    if(eventHandler && eventHandler->hasAction()){
        eventHandler->back();
    }
}

void PF_GraphicView::setContainer(PF_EntityContainer *_container)
{
    this->container = _container;
}

void PF_GraphicView::setCurrentAction(PF_ActionInterface *action)
{
    //qDebug()<<"PF_GraphicView::setCurrentAction";
    if(eventHandler){
        eventHandler->setCurrentAction(action);
    }
    //qDebug()<<"PF_GraphicView::setCurrentAction: OK.";
}

PF_ActionInterface *PF_GraphicView::getCurrentAction()
{
    if(eventHandler){
        return eventHandler->getCurrentAction();
    }else{
        return nullptr;
    }
}

void PF_GraphicView::setDefaultAction(PF_ActionInterface *action)
{
    if(eventHandler){
        eventHandler->setDefaultAction(action);
    }
}

PF_ActionInterface *PF_GraphicView::getDefaultAction()
{
    if(eventHandler){
        return eventHandler->getDefaultAction();
    }else{
        return nullptr;
    }
}

void PF_GraphicView::redraw(PF::RedrawMethod method)
{
    redrawMethod = (PF::RedrawMethod ) (redrawMethod | method);
    update(); // Paint when reeady to pain
    repaint(); //Paint immediate
}

void PF_GraphicView::drawEntity(QCPPainter *painter, PF_Entity *e)
{
    if(!e){
        return;
    }
    e->draw(painter);
}

//void PF_GraphicView::drawEntityLayer(QPainter *painter)
//{
//    drawEntity(painter, container);
//}

//void PF_GraphicView::drawLayer3(QPainter *painter)
//{
//    drawOverlay(painter);
//}

void PF_GraphicView::drawOverlay(QCPPainter *painter)
{
    foreach (auto ec, overlayEntities)
    {
        foreach (auto e, ec->getEntityList())
        {
            //setPenForEntity(painter, e);
            e->draw(painter);
        }
    }
}



/*!
  Sets which elements are forcibly drawn antialiased as an \a or combination of QCP::AntialiasedElement.

  This overrides the antialiasing settings for whole element groups, normally controlled with the
  \a setAntialiasing function on the individual elements. If an element is neither specified in
  \ref setAntialiasedElements nor in \ref setNotAntialiasedElements, the antialiasing setting on
  each individual element instance is used.

  For example, if \a antialiasedElements contains \ref QCP::aePlottables, all plottables will be
  drawn antialiased, no matter what the specific QCPAbstractPlottable::setAntialiased value was set
  to.

  if an element in \a antialiasedElements is already set in \ref setNotAntialiasedElements, it is
  removed from there.

  \see setNotAntialiasedElements
*/
void PF_GraphicView::setAntialiasedElements(const QCP::AntialiasedElements &antialiasedElements)
{
    mAntialiasedElements = antialiasedElements;

    // make sure elements aren't in mNotAntialiasedElements and mAntialiasedElements simultaneously:
    if ((mNotAntialiasedElements & mAntialiasedElements) != 0)
        mNotAntialiasedElements |= ~mAntialiasedElements;
}

/*!
  Sets whether the specified \a antialiasedElement is forcibly drawn antialiased.

  See \ref setAntialiasedElements for details.

  \see setNotAntialiasedElement
*/
void PF_GraphicView::setAntialiasedElement(QCP::AntialiasedElement antialiasedElement, bool enabled)
{
    if (!enabled && mAntialiasedElements.testFlag(antialiasedElement))
        mAntialiasedElements &= ~antialiasedElement;
    else if (enabled && !mAntialiasedElements.testFlag(antialiasedElement))
        mAntialiasedElements |= antialiasedElement;

    // make sure elements aren't in mNotAntialiasedElements and mAntialiasedElements simultaneously:
    if ((mNotAntialiasedElements & mAntialiasedElements) != 0)
        mNotAntialiasedElements |= ~mAntialiasedElements;
}

/*!
  Sets which elements are forcibly drawn not antialiased as an \a or combination of
  QCP::AntialiasedElement.

  This overrides the antialiasing settings for whole element groups, normally controlled with the
  \a setAntialiasing function on the individual elements. If an element is neither specified in
  \ref setAntialiasedElements nor in \ref setNotAntialiasedElements, the antialiasing setting on
  each individual element instance is used.

  For example, if \a notAntialiasedElements contains \ref QCP::aePlottables, no plottables will be
  drawn antialiased, no matter what the specific QCPAbstractPlottable::setAntialiased value was set
  to.

  if an element in \a notAntialiasedElements is already set in \ref setAntialiasedElements, it is
  removed from there.

  \see setAntialiasedElements
*/
void PF_GraphicView::setNotAntialiasedElements(const QCP::AntialiasedElements &notAntialiasedElements)
{
    mNotAntialiasedElements = notAntialiasedElements;

    // make sure elements aren't in mNotAntialiasedElements and mAntialiasedElements simultaneously:
    if ((mNotAntialiasedElements & mAntialiasedElements) != 0)
        mAntialiasedElements |= ~mNotAntialiasedElements;
}

/*!
  Sets whether the specified \a notAntialiasedElement is forcibly drawn not antialiased.

  See \ref setNotAntialiasedElements for details.

  \see setAntialiasedElement
*/
void PF_GraphicView::setNotAntialiasedElement(QCP::AntialiasedElement notAntialiasedElement, bool enabled)
{
    if (!enabled && mNotAntialiasedElements.testFlag(notAntialiasedElement))
        mNotAntialiasedElements &= ~notAntialiasedElement;
    else if (enabled && !mNotAntialiasedElements.testFlag(notAntialiasedElement))
        mNotAntialiasedElements |= notAntialiasedElement;

    // make sure elements aren't in mNotAntialiasedElements and mAntialiasedElements simultaneously:
    if ((mNotAntialiasedElements & mAntialiasedElements) != 0)
        mAntialiasedElements |= ~mNotAntialiasedElements;
}

/*!
  If set to true, adding a plottable (e.g. a graph) to the PF_GraphicView automatically also adds the
  plottable to the legend (PF_GraphicView::legend).

  \see addGraph, QCPLegend::addItem
*/
void PF_GraphicView::setAutoAddPlottableToLegend(bool on)
{
    mAutoAddPlottableToLegend = on;
}

/*!
  Sets the possible interactions of this PF_GraphicView as an or-combination of \ref QCP::Interaction
  enums. There are the following types of interactions:

  <b>Axis range manipulation</b> is controlled via \ref QCP::iRangeDrag and \ref QCP::iRangeZoom. When the
  respective interaction is enabled, the user may drag axes ranges and zoom with the mouse wheel.
  For details how to control which axes the user may drag/zoom and in what orientations, see \ref
  QCPAxisRect::setRangeDrag, \ref QCPAxisRect::setRangeZoom, \ref QCPAxisRect::setRangeDragAxes,
  \ref QCPAxisRect::setRangeZoomAxes.

  <b>Plottable data selection</b> is controlled by \ref QCP::iSelectPlottables. If \ref
  QCP::iSelectPlottables is set, the user may select plottables (graphs, curves, bars,...) and
  their data by clicking on them or in their vicinity (\ref setSelectionTolerance). Whether the
  user can actually select a plottable and its data can further be restricted with the \ref
  QCPAbstractPlottable::setSelectable method on the specific plottable. For details, see the
  special page about the \ref dataselection "data selection mechanism". To retrieve a list of all
  currently selected plottables, call \ref selectedPlottables. If you're only interested in
  QCPGraphs, you may use the convenience function \ref selectedGraphs.

  <b>Item selection</b> is controlled by \ref QCP::iSelectItems. If \ref QCP::iSelectItems is set, the user
  may select items (QCPItemLine, QCPItemText,...) by clicking on them or in their vicinity. To find
  out whether a specific item is selected, call QCPAbstractItem::selected(). To retrieve a list of
  all currently selected items, call \ref selectedItems.

  <b>Axis selection</b> is controlled with \ref QCP::iSelectAxes. If \ref QCP::iSelectAxes is set, the user
  may select parts of the axes by clicking on them. What parts exactly (e.g. Axis base line, tick
  labels, axis label) are selectable can be controlled via \ref QCPAxis::setSelectableParts for
  each axis. To retrieve a list of all axes that currently contain selected parts, call \ref
  selectedAxes. Which parts of an axis are selected, can be retrieved with QCPAxis::selectedParts().

  <b>Legend selection</b> is controlled with \ref QCP::iSelectLegend. If this is set, the user may
  select the legend itself or individual items by clicking on them. What parts exactly are
  selectable can be controlled via \ref QCPLegend::setSelectableParts. To find out whether the
  legend or any of its child items are selected, check the value of QCPLegend::selectedParts. To
  find out which child items are selected, call \ref QCPLegend::selectedItems.

  <b>All other selectable elements</b> The selection of all other selectable objects (e.g.
  QCPTextElement, or your own layerable subclasses) is controlled with \ref QCP::iSelectOther. If set, the
  user may select those objects by clicking on them. To find out which are currently selected, you
  need to check their selected state explicitly.

  If the selection state has changed by user interaction, the \ref selectionChangedByUser signal is
  emitted. Each selectable object additionally emits an individual selectionChanged signal whenever
  their selection state has changed, i.e. not only by user interaction.

  To allow multiple objects to be selected by holding the selection modifier (\ref
  setMultiSelectModifier), set the flag \ref QCP::iMultiSelect.

  \note In addition to the selection mechanism presented here, PF_GraphicView always emits
  corresponding signals, when an object is clicked or double clicked. see \ref plottableClick and
  \ref plottableDoubleClick for example.

  \see setInteraction, setSelectionTolerance
*/
void PF_GraphicView::setInteractions(const QCP::Interactions &interactions)
{
    mInteractions = interactions;
}

/*!
  Sets the single \a interaction of this PF_GraphicView to \a enabled.

  For details about the interaction system, see \ref setInteractions.

  \see setInteractions
*/
void PF_GraphicView::setInteraction(const QCP::Interaction &interaction, bool enabled)
{
    if (!enabled && mInteractions.testFlag(interaction))
        mInteractions &= ~interaction;
    else if (enabled && !mInteractions.testFlag(interaction))
        mInteractions |= interaction;
}

/*!
  Sets the tolerance that is used to decide whether a click selects an object (e.g. a plottable) or
  not.

  If the user clicks in the vicinity of the line of e.g. a QCPGraph, it's only regarded as a
  potential selection when the minimum distance between the click position and the graph line is
  smaller than \a pixels. Objects that are defined by an area (e.g. QCPBars) only react to clicks
  directly inside the area and ignore this selection tolerance. In other words, it only has meaning
  for parts of objects that are too thin to exactly hit with a click and thus need such a
  tolerance.

  \see setInteractions, QCPLayerable::selectTest
*/
void PF_GraphicView::setSelectionTolerance(int pixels)
{
    mSelectionTolerance = pixels;
}

/*!
  Sets whether antialiasing is disabled for this PF_GraphicView while the user is dragging axes
  ranges. If many objects, especially plottables, are drawn antialiased, this greatly improves
  performance during dragging. Thus it creates a more responsive user experience. As soon as the
  user stops dragging, the last replot is done with normal antialiasing, to restore high image
  quality.

  \see setAntialiasedElements, setNotAntialiasedElements
*/
void PF_GraphicView::setNoAntialiasingOnDrag(bool enabled)
{
    mNoAntialiasingOnDrag = enabled;
}

/*!
  Sets the plotting hints for this PF_GraphicView instance as an \a or combination of QCP::PlottingHint.

  \see setPlottingHint
*/
void PF_GraphicView::setPlottingHints(const QCP::PlottingHints &hints)
{
    mPlottingHints = hints;
}

/*!
  Sets the specified plotting \a hint to \a enabled.

  \see setPlottingHints
*/
void PF_GraphicView::setPlottingHint(QCP::PlottingHint hint, bool enabled)
{
    QCP::PlottingHints newHints = mPlottingHints;
    if (!enabled)
        newHints &= ~hint;
    else
        newHints |= hint;

    if (newHints != mPlottingHints)
        setPlottingHints(newHints);
}

/*!
  Sets the keyboard modifier that will be recognized as multi-select-modifier.

  If \ref QCP::iMultiSelect is specified in \ref setInteractions, the user may select multiple
  objects (or data points) by clicking on them one after the other while holding down \a modifier.

  By default the multi-select-modifier is set to Qt::ControlModifier.

  \see setInteractions
*/
void PF_GraphicView::setMultiSelectModifier(Qt::KeyboardModifier modifier)
{
    mMultiSelectModifier = modifier;
}

/*!
  Sets how PF_GraphicView processes mouse click-and-drag interactions by the user.

  If \a mode is \ref QCP::srmNone, the mouse drag is forwarded to the underlying objects. For
  example, QCPAxisRect may process a mouse drag by dragging axis ranges, see \ref
  QCPAxisRect::setRangeDrag. If \a mode is not \ref QCP::srmNone, the current selection rect (\ref
  selectionRect) becomes activated and allows e.g. rect zooming and data point selection.

  If you wish to provide your user both with axis range dragging and data selection/range zooming,
  use this method to switch between the modes just before the interaction is processed, e.g. in
  reaction to the \ref mousePress or \ref mouseMove signals. For example you could check whether
  the user is holding a certain keyboard modifier, and then decide which \a mode shall be set.

  If a selection rect interaction is currently active, and \a mode is set to \ref QCP::srmNone, the
  interaction is canceled (\ref QCPSelectionRect::cancel). Switching between any of the other modes
  will keep the selection rect active. Upon completion of the interaction, the behaviour is as
  defined by the currently set \a mode, not the mode that was set when the interaction started.

  \see setInteractions, setSelectionRect, QCPSelectionRect
*/
void PF_GraphicView::setSelectionRectMode(QCP::SelectionRectMode mode)
{
    if (mSelectionRect)
    {
        if (mode == QCP::srmNone)
            mSelectionRect->cancel(); // when switching to none, we immediately want to abort a potentially active selection rect

        // disconnect old connections:
        if (mSelectionRectMode == QCP::srmSelect)
            disconnect(mSelectionRect, SIGNAL(accepted(QRect,QMouseEvent*)), this, SLOT(processRectSelection(QRect,QMouseEvent*)));
        else if (mSelectionRectMode == QCP::srmZoom)
            disconnect(mSelectionRect, SIGNAL(accepted(QRect,QMouseEvent*)), this, SLOT(processRectZoom(QRect,QMouseEvent*)));

        // establish new ones:
        if (mode == QCP::srmSelect)
            connect(mSelectionRect, SIGNAL(accepted(QRect,QMouseEvent*)), this, SLOT(processRectSelection(QRect,QMouseEvent*)));
        else if (mode == QCP::srmZoom)
            connect(mSelectionRect, SIGNAL(accepted(QRect,QMouseEvent*)), this, SLOT(processRectZoom(QRect,QMouseEvent*)));
    }

    mSelectionRectMode = mode;
}

/*!
  Sets the \ref QCPSelectionRect instance that PF_GraphicView will use if \a mode is not \ref
  QCP::srmNone and the user performs a click-and-drag interaction. PF_GraphicView takes ownership of
  the passed \a selectionRect. It can be accessed later via \ref selectionRect.

  This method is useful if you wish to replace the default QCPSelectionRect instance with an
  instance of a QCPSelectionRect subclass, to introduce custom behaviour of the selection rect.

  \see setSelectionRectMode
*/
void PF_GraphicView::setSelectionRect(QCPSelectionRect *selectionRect)
{
    if (mSelectionRect)
        delete mSelectionRect;

    mSelectionRect = selectionRect;

    if (mSelectionRect)
    {
        // establish connections with new selection rect:
        if (mSelectionRectMode == QCP::srmSelect)
            connect(mSelectionRect, SIGNAL(accepted(QRect,QMouseEvent*)), this, SLOT(processRectSelection(QRect,QMouseEvent*)));
        else if (mSelectionRectMode == QCP::srmZoom)
            connect(mSelectionRect, SIGNAL(accepted(QRect,QMouseEvent*)), this, SLOT(processRectZoom(QRect,QMouseEvent*)));
    }
}

/*!
  \warning This is still an experimental feature and its performance depends on the system that it
  runs on. Having multiple PF_GraphicView widgets in one application with enabled OpenGL rendering
  might cause context conflicts on some systems.

  This method allows to enable OpenGL plot rendering, for increased plotting performance of
  graphically demanding plots (thick lines, translucent fills, etc.).

  If \a enabled is set to true, PF_GraphicView will try to initialize OpenGL and, if successful,
  continue plotting with hardware acceleration. The parameter \a multisampling controls how many
  samples will be used per pixel, it essentially controls the antialiasing quality. If \a
  multisampling is set too high for the current graphics hardware, the maximum allowed value will
  be used.

  You can test whether switching to OpenGL rendering was successful by checking whether the
  according getter \a PF_GraphicView::openGl() returns true. If the OpenGL initialization fails,
  rendering continues with the regular software rasterizer, and an according qDebug output is
  generated.

  If switching to OpenGL was successful, this method disables label caching (\ref setPlottingHint
  "setPlottingHint(QCP::phCacheLabels, false)") and turns on PF_GraphicView's antialiasing override
  for all elements (\ref setAntialiasedElements "setAntialiasedElements(QCP::aeAll)"), leading to a
  higher quality output. The antialiasing override allows for pixel-grid aligned drawing in the
  OpenGL paint device. As stated before, in OpenGL rendering the actual antialiasing of the plot is
  controlled with \a multisampling. If \a enabled is set to false, the antialiasing/label caching
  settings are restored to what they were before OpenGL was enabled, if they weren't altered in the
  meantime.

  \note OpenGL support is only enabled if PF_GraphicView is compiled with the macro \c PF_GraphicView_USE_OPENGL
  defined. This define must be set before including the PF_GraphicView header both during compilation
  of the PF_GraphicView library as well as when compiling your application. It is best to just include
  the line <tt>DEFINES += PF_GraphicView_USE_OPENGL</tt> in the respective qmake project files.
  \note If you are using a Qt version before 5.0, you must also add the module "opengl" to your \c
  QT variable in the qmake project files. For Qt versions 5.0 and higher, PF_GraphicView switches to a
  newer OpenGL interface which is already in the "gui" module.
*/
void PF_GraphicView::setOpenGl(bool enabled, int multisampling)
{
    mOpenGlMultisamples = qMax(0, multisampling);
#ifdef PF_GraphicView_USE_OPENGL
    mOpenGl = enabled;
    if (mOpenGl)
    {
        if (setupOpenGl())
        {
            // backup antialiasing override and labelcaching setting so we can restore upon disabling OpenGL
            mOpenGlAntialiasedElementsBackup = mAntialiasedElements;
            mOpenGlCacheLabelsBackup = mPlottingHints.testFlag(QCP::phCacheLabels);
            // set antialiasing override to antialias all (aligns gl pixel grid properly), and disable label caching (would use software rasterizer for pixmap caches):
            setAntialiasedElements(QCP::aeAll);
            setPlottingHint(QCP::phCacheLabels, false);
        } else
        {
            qDebug() << Q_FUNC_INFO << "Failed to enable OpenGL, continuing plotting without hardware acceleration.";
            mOpenGl = false;
        }
    } else
    {
        // restore antialiasing override and labelcaching to what it was before enabling OpenGL, if nobody changed it in the meantime:
        if (mAntialiasedElements == QCP::aeAll)
            setAntialiasedElements(mOpenGlAntialiasedElementsBackup);
        if (!mPlottingHints.testFlag(QCP::phCacheLabels))
            setPlottingHint(QCP::phCacheLabels, mOpenGlCacheLabelsBackup);
        freeOpenGl();
    }
    // recreate all paint buffers:
    mPaintBuffers.clear();
    setupPaintBuffers();
#else
    Q_UNUSED(enabled)
    qDebug() << Q_FUNC_INFO << "PF_GraphicView can't use OpenGL because PF_GraphicView_USE_OPENGL was not defined during compilation (add 'DEFINES += PF_GraphicView_USE_OPENGL' to your qmake .pro file)";
#endif
}

/*!
  Sets the viewport of this PF_GraphicView. Usually users of PF_GraphicView don't need to change the
  viewport manually.

  The viewport is the area in which the plot is drawn. All mechanisms, e.g. margin caluclation take
  the viewport to be the outer border of the plot. The viewport normally is the rect() of the
  PF_GraphicView widget, i.e. a rect with top left (0, 0) and size of the PF_GraphicView widget.

  Don't confuse the viewport with the axis rect (PF_GraphicView::axisRect). An axis rect is typically
  an area enclosed by four axes, where the graphs/plottables are drawn in. The viewport is larger
  and contains also the axes themselves, their tick numbers, their labels, or even additional axis
  rects, color scales and other layout elements.

  This function is used to allow arbitrary size exports with \ref toPixmap, \ref savePng, \ref
  savePdf, etc. by temporarily changing the viewport size.
*/
void PF_GraphicView::setViewport(const QRect &rect)
{
    mViewport = rect;
    if (mPlotLayout)
        mPlotLayout->setOuterRect(mViewport);
}

/*!
  Sets the device pixel ratio used by the paint buffers of this PF_GraphicView instance.

  Normally, this doesn't need to be set manually, because it is initialized with the regular \a
  QWidget::devicePixelRatio which is configured by Qt to fit the display device (e.g. 1 for normal
  displays, 2 for High-DPI displays).

  Device pixel ratios are supported by Qt only for Qt versions since 5.4. If this method is called
  when PF_GraphicView is being used with older Qt versions, outputs an according qDebug message and
  leaves the internal buffer device pixel ratio at 1.0.
*/
void PF_GraphicView::setBufferDevicePixelRatio(double ratio)
{
    if (!qFuzzyCompare(ratio, mBufferDevicePixelRatio))
    {
#ifdef QCP_DEVICEPIXELRATIO_SUPPORTED
        mBufferDevicePixelRatio = ratio;
        for (int i=0; i<mPaintBuffers.size(); ++i)
            mPaintBuffers.at(i)->setDevicePixelRatio(mBufferDevicePixelRatio);
        // Note: axis label cache has devicePixelRatio as part of cache hash, so no need to manually clear cache here
#else
        qDebug() << Q_FUNC_INFO << "Device pixel ratios not supported for Qt versions before 5.4";
        mBufferDevicePixelRatio = 1.0;
#endif
    }
}

/*!
  Sets \a pm as the viewport background pixmap (see \ref setViewport). The pixmap is always drawn
  below all other objects in the plot.

  For cases where the provided pixmap doesn't have the same size as the viewport, scaling can be
  enabled with \ref setBackgroundScaled and the scaling mode (whether and how the aspect ratio is
  preserved) can be set with \ref setBackgroundScaledMode. To set all these options in one call,
  consider using the overloaded version of this function.

  If a background brush was set with \ref setBackground(const QBrush &brush), the viewport will
  first be filled with that brush, before drawing the background pixmap. This can be useful for
  background pixmaps with translucent areas.

  \see setBackgroundScaled, setBackgroundScaledMode
*/
void PF_GraphicView::setBackground(const QPixmap &pm)
{
    mBackgroundPixmap = pm;
    mScaledBackgroundPixmap = QPixmap();
}

/*!
  Sets the background brush of the viewport (see \ref setViewport).

  Before drawing everything else, the background is filled with \a brush. If a background pixmap
  was set with \ref setBackground(const QPixmap &pm), this brush will be used to fill the viewport
  before the background pixmap is drawn. This can be useful for background pixmaps with translucent
  areas.

  Set \a brush to Qt::NoBrush or Qt::Transparent to leave background transparent. This can be
  useful for exporting to image formats which support transparency, e.g. \ref savePng.

  \see setBackgroundScaled, setBackgroundScaledMode
*/
void PF_GraphicView::setBackground(const QBrush &brush)
{
    mBackgroundBrush = brush;
}

/*! \overload

  Allows setting the background pixmap of the viewport, whether it shall be scaled and how it
  shall be scaled in one call.

  \see setBackground(const QPixmap &pm), setBackgroundScaled, setBackgroundScaledMode
*/
void PF_GraphicView::setBackground(const QPixmap &pm, bool scaled, Qt::AspectRatioMode mode)
{
    mBackgroundPixmap = pm;
    mScaledBackgroundPixmap = QPixmap();
    mBackgroundScaled = scaled;
    mBackgroundScaledMode = mode;
}

/*!
  Sets whether the viewport background pixmap shall be scaled to fit the viewport. If \a scaled is
  set to true, control whether and how the aspect ratio of the original pixmap is preserved with
  \ref setBackgroundScaledMode.

  Note that the scaled version of the original pixmap is buffered, so there is no performance
  penalty on replots. (Except when the viewport dimensions are changed continuously.)

  \see setBackground, setBackgroundScaledMode
*/
void PF_GraphicView::setBackgroundScaled(bool scaled)
{
    mBackgroundScaled = scaled;
}

/*!
  If scaling of the viewport background pixmap is enabled (\ref setBackgroundScaled), use this
  function to define whether and how the aspect ratio of the original pixmap is preserved.

  \see setBackground, setBackgroundScaled
*/
void PF_GraphicView::setBackgroundScaledMode(Qt::AspectRatioMode mode)
{
    mBackgroundScaledMode = mode;
}

/*!
  Returns the plottable with \a index. If the index is invalid, returns 0.

  There is an overloaded version of this function with no parameter which returns the last added
  plottable, see PF_GraphicView::plottable()

  \see plottableCount
*/
//QCPAbstractPlottable *PF_GraphicView::plottable(int index)
//{
//    if (index >= 0 && index < mPlottables.size())
//    {
//        return mPlottables.at(index);
//    } else
//    {
//        qDebug() << Q_FUNC_INFO << "index out of bounds:" << index;
//        return nullptr;
//    }
//}

/*! \overload

  Returns the last plottable that was added to the plot. If there are no plottables in the plot,
  returns 0.

  \see plottableCount
*/
//QCPAbstractPlottable *PF_GraphicView::plottable()
//{
//    if (!mPlottables.isEmpty())
//    {
//        return mPlottables.last();
//    } else
//        return nullptr;
//}

/*!
  Removes the specified plottable from the plot and deletes it. If necessary, the corresponding
  legend item is also removed from the default legend (PF_GraphicView::legend).

  Returns true on success.

  \see clearPlottables
*/
//bool PF_GraphicView::removePlottable(QCPAbstractPlottable *plottable)
//{
//    if (!mPlottables.contains(plottable))
//    {
//        qDebug() << Q_FUNC_INFO << "plottable not in list:" << reinterpret_cast<quintptr>(plottable);
//        return false;
//    }

//    // remove plottable from legend:
//    plottable->removeFromLegend();
//    // special handling for QCPGraphs to maintain the simple graph interface:
//    if (QCPGraph *graph = qobject_cast<QCPGraph*>(plottable))
//        mGraphs.removeOne(graph);
//    // remove plottable:
//    delete plottable;
//    mPlottables.removeOne(plottable);
//    return true;
//}

/*! \overload

  Removes and deletes the plottable by its \a index.
*/
//bool PF_GraphicView::removePlottable(int index)
//{
//    if (index >= 0 && index < mPlottables.size())
//        return removePlottable(mPlottables[index]);
//    else
//    {
//        qDebug() << Q_FUNC_INFO << "index out of bounds:" << index;
//        return false;
//    }
//}

/*!
  Removes all plottables from the plot and deletes them. Corresponding legend items are also
  removed from the default legend (PF_GraphicView::legend).

  Returns the number of plottables removed.

  \see removePlottable
*/
//int PF_GraphicView::clearPlottables()
//{
//    int c = mPlottables.size();
//    for (int i=c-1; i >= 0; --i)
//        removePlottable(mPlottables[i]);
//    return c;
//}

/*!
  Returns the number of currently existing plottables in the plot

  \see plottable
*/
//int PF_GraphicView::plottableCount() const
//{
//    return mPlottables.size();
//}

/*!
  Returns a list of the selected plottables. If no plottables are currently selected, the list is empty.

  There is a convenience function if you're only interested in selected graphs, see \ref selectedGraphs.

  \see setInteractions, QCPAbstractPlottable::setSelectable, QCPAbstractPlottable::setSelection
*/
//QList<QCPAbstractPlottable*> PF_GraphicView::selectedPlottables() const
//{
//    QList<QCPAbstractPlottable*> result;
//    foreach (QCPAbstractPlottable *plottable, mPlottables)
//    {
//        if (plottable->selected())
//            result.append(plottable);
//    }
//    return result;
//}

/*!
  Returns the plottable at the pixel position \a pos. Plottables that only consist of single lines
  (like graphs) have a tolerance band around them, see \ref setSelectionTolerance. If multiple
  plottables come into consideration, the one closest to \a pos is returned.

  If \a onlySelectable is true, only plottables that are selectable
  (QCPAbstractPlottable::setSelectable) are considered.

  If there is no plottable at \a pos, the return value is 0.

  \see itemAt, layoutElementAt
*/
//QCPAbstractPlottable *PF_GraphicView::plottableAt(const QPointF &pos, bool onlySelectable) const
//{
//    QCPAbstractPlottable *resultPlottable = nullptr;
//    double resultDistance = mSelectionTolerance; // only regard clicks with distances smaller than mSelectionTolerance as selections, so initialize with that value

//    foreach (QCPAbstractPlottable *plottable, mPlottables)
//    {
//        if (onlySelectable && !plottable->selectable()) // we could have also passed onlySelectable to the selectTest function, but checking here is faster, because we have access to QCPabstractPlottable::selectable
//            continue;
//        if ((plottable->keyAxis()->axisRect()->rect() & plottable->valueAxis()->axisRect()->rect()).contains(pos.toPoint())) // only consider clicks inside the rect that is spanned by the plottable's key/value axes
//        {
//            double currentDistance = plottable->selectTest(pos, false);
//            if (currentDistance >= 0 && currentDistance < resultDistance)
//            {
//                resultPlottable = plottable;
//                resultDistance = currentDistance;
//            }
//        }
//    }

//    return resultPlottable;
//}

/*!
  Returns whether this PF_GraphicView instance contains the \a plottable.
*/
//bool PF_GraphicView::hasPlottable(QCPAbstractPlottable *plottable) const
//{
//    return mPlottables.contains(plottable);
//}

/*!
  Returns the graph with \a index. If the index is invalid, returns 0.

  There is an overloaded version of this function with no parameter which returns the last created
  graph, see PF_GraphicView::graph()

  \see graphCount, addGraph
*/
//QCPGraph *PF_GraphicView::graph(int index) const
//{
//    if (index >= 0 && index < mGraphs.size())
//    {
//        return mGraphs.at(index);
//    } else
//    {
//        qDebug() << Q_FUNC_INFO << "index out of bounds:" << index;
//        return nullptr;
//    }
//}

/*! \overload

  Returns the last graph, that was created with \ref addGraph. If there are no graphs in the plot,
  returns 0.

  \see graphCount, addGraph
*/
//QCPGraph *PF_GraphicView::graph() const
//{
//    if (!mGraphs.isEmpty())
//    {
//        return mGraphs.last();
//    } else
//        return nullptr;
//}

/*!
  Creates a new graph inside the plot. If \a keyAxis and \a valueAxis are left unspecified (0), the
  bottom (xAxis) is used as key and the left (yAxis) is used as value axis. If specified, \a
  keyAxis and \a valueAxis must reside in this PF_GraphicView.

  \a keyAxis will be used as key axis (typically "x") and \a valueAxis as value axis (typically
  "y") for the graph.

  Returns a pointer to the newly created graph, or 0 if adding the graph failed.

  \see graph, graphCount, removeGraph, clearGraphs
*/
//QCPGraph *PF_GraphicView::addGraph(QCPAxis *keyAxis, QCPAxis *valueAxis)
//{
//    if (!keyAxis) keyAxis = xAxis;
//    if (!valueAxis) valueAxis = yAxis;
//    if (!keyAxis || !valueAxis)
//    {
//        qDebug() << Q_FUNC_INFO << "can't use default PF_GraphicView xAxis or yAxis, because at least one is invalid (has been deleted)";
//        return nullptr;
//    }
//    if (keyAxis->parentPlot() != this || valueAxis->parentPlot() != this)
//    {
//        qDebug() << Q_FUNC_INFO << "passed keyAxis or valueAxis doesn't have this PF_GraphicView as parent";
//        return nullptr;
//    }

//    QCPGraph *newGraph = new QCPGraph(keyAxis, valueAxis);
//    newGraph->setName(QLatin1String("Graph ")+QString::number(mGraphs.size()));
//    return newGraph;
//}

/*!
  Removes the specified \a graph from the plot and deletes it. If necessary, the corresponding
  legend item is also removed from the default legend (PF_GraphicView::legend). If any other graphs in
  the plot have a channel fill set towards the removed graph, the channel fill property of those
  graphs is reset to zero (no channel fill).

  Returns true on success.

  \see clearGraphs
*/
//bool PF_GraphicView::removeGraph(QCPGraph *graph)
//{
//    return removePlottable(graph);
//}

/*! \overload

  Removes and deletes the graph by its \a index.
*/
//bool PF_GraphicView::removeGraph(int index)
//{
//    if (index >= 0 && index < mGraphs.size())
//        return removeGraph(mGraphs[index]);
//    else
//        return false;
//}

/*!
  Removes all graphs from the plot and deletes them. Corresponding legend items are also removed
  from the default legend (PF_GraphicView::legend).

  Returns the number of graphs removed.

  \see removeGraph
*/
//int PF_GraphicView::clearGraphs()
//{
//    int c = mGraphs.size();
//    for (int i=c-1; i >= 0; --i)
//        removeGraph(mGraphs[i]);
//    return c;
//}

/*!
  Returns the number of currently existing graphs in the plot

  \see graph, addGraph
*/
//int PF_GraphicView::graphCount() const
//{
//    return mGraphs.size();
//}

/*!
  Returns a list of the selected graphs. If no graphs are currently selected, the list is empty.

  If you are not only interested in selected graphs but other plottables like QCPCurve, QCPBars,
  etc., use \ref selectedPlottables.

  \see setInteractions, selectedPlottables, QCPAbstractPlottable::setSelectable, QCPAbstractPlottable::setSelection
*/
//QList<QCPGraph*> PF_GraphicView::selectedGraphs() const
//{
//    QList<QCPGraph*> result;
//    foreach (QCPGraph *graph, mGraphs)
//    {
//        if (graph->selected())
//            result.append(graph);
//    }
//    return result;
//}

/*!
  Returns the item with \a index. If the index is invalid, returns 0.

  There is an overloaded version of this function with no parameter which returns the last added
  item, see PF_GraphicView::item()

  \see itemCount
*/
QCPAbstractItem *PF_GraphicView::item(int index) const
{
    if (index >= 0 && index < mItems.size())
    {
        return mItems.at(index);
    } else
    {
        qDebug() << Q_FUNC_INFO << "index out of bounds:" << index;
        return nullptr;
    }
}

/*! \overload

  Returns the last item that was added to this plot. If there are no items in the plot,
  returns 0.

  \see itemCount
*/
QCPAbstractItem *PF_GraphicView::item() const
{
    if (!mItems.isEmpty())
    {
        return mItems.last();
    } else
        return nullptr;
}

/*!
  Removes the specified item from the plot and deletes it.

  Returns true on success.

  \see clearItems
*/
bool PF_GraphicView::removeItem(QCPAbstractItem *item)
{
    if (mItems.contains(item))
    {
        delete item;
        mItems.removeOne(item);
        return true;
    } else
    {
        qDebug() << Q_FUNC_INFO << "item not in list:" << reinterpret_cast<quintptr>(item);
        return false;
    }
}

/*! \overload

  Removes and deletes the item by its \a index.
*/
bool PF_GraphicView::removeItem(int index)
{
    if (index >= 0 && index < mItems.size())
        return removeItem(mItems[index]);
    else
    {
        qDebug() << Q_FUNC_INFO << "index out of bounds:" << index;
        return false;
    }
}

/*!
  Removes all items from the plot and deletes them.

  Returns the number of items removed.

  \see removeItem
*/
int PF_GraphicView::clearItems()
{
    int c = mItems.size();
    for (int i=c-1; i >= 0; --i)
        removeItem(mItems[i]);
    return c;
}

/*!
  Returns the number of currently existing items in the plot

  \see item
*/
int PF_GraphicView::itemCount() const
{
    return mItems.size();
}

/*!
  Returns a list of the selected items. If no items are currently selected, the list is empty.

  \see setInteractions, QCPAbstractItem::setSelectable, QCPAbstractItem::setSelected
*/
QList<QCPAbstractItem*> PF_GraphicView::selectedItems() const
{
    QList<QCPAbstractItem*> result;
    foreach (QCPAbstractItem *item, mItems)
    {
        if (item->selected())
            result.append(item);
    }
    return result;
}

/*!
  Returns the item at the pixel position \a pos. Items that only consist of single lines (e.g. \ref
  QCPItemLine or \ref QCPItemCurve) have a tolerance band around them, see \ref
  setSelectionTolerance. If multiple items come into consideration, the one closest to \a pos is
  returned.

  If \a onlySelectable is true, only items that are selectable (QCPAbstractItem::setSelectable) are
  considered.

  If there is no item at \a pos, the return value is 0.

  \see plottableAt, layoutElementAt
*/
QCPAbstractItem *PF_GraphicView::itemAt(const QPointF &pos, bool onlySelectable) const
{
    QCPAbstractItem *resultItem = nullptr;
    double resultDistance = mSelectionTolerance; // only regard clicks with distances smaller than mSelectionTolerance as selections, so initialize with that value

    foreach (QCPAbstractItem *item, mItems)
    {
        if (onlySelectable && !item->selectable()) // we could have also passed onlySelectable to the selectTest function, but checking here is faster, because we have access to QCPAbstractItem::selectable
            continue;
        if (!item->clipToAxisRect() || item->clipRect().contains(pos.toPoint())) // only consider clicks inside axis cliprect of the item if actually clipped to it
        {
            double currentDistance = item->selectTest(pos, false);
            if (currentDistance >= 0 && currentDistance < resultDistance)
            {
                resultItem = item;
                resultDistance = currentDistance;
            }
        }
    }

    return resultItem;
}

/*!
  Returns whether this PF_GraphicView contains the \a item.

  \see item
*/
bool PF_GraphicView::hasItem(QCPAbstractItem *item) const
{
    return mItems.contains(item);
}

/*!
  Returns the layer with the specified \a name. If there is no layer with the specified name, 0 is
  returned.

  Layer names are case-sensitive.

  \see addLayer, moveLayer, removeLayer
*/
QCPLayer *PF_GraphicView::layer(const QString &name) const
{
    foreach (QCPLayer *layer, mLayers)
    {
        if (layer->name() == name)
            return layer;
    }
    return nullptr;
}

/*! \overload

  Returns the layer by \a index. If the index is invalid, 0 is returned.

  \see addLayer, moveLayer, removeLayer
*/
QCPLayer *PF_GraphicView::layer(int index) const
{
    if (index >= 0 && index < mLayers.size())
    {
        return mLayers.at(index);
    } else
    {
        qDebug() << Q_FUNC_INFO << "index out of bounds:" << index;
        return nullptr;
    }
}

/*!
  Returns the layer that is set as current layer (see \ref setCurrentLayer).
*/
QCPLayer *PF_GraphicView::currentLayer() const
{
    return mCurrentLayer;
}

/*!
  Sets the layer with the specified \a name to be the current layer. All layerables (\ref
  QCPLayerable), e.g. plottables and items, are created on the current layer.

  Returns true on success, i.e. if there is a layer with the specified \a name in the PF_GraphicView.

  Layer names are case-sensitive.

  \see addLayer, moveLayer, removeLayer, QCPLayerable::setLayer
*/
bool PF_GraphicView::setCurrentLayer(const QString &name)
{
    if (QCPLayer *newCurrentLayer = layer(name))
    {
        return setCurrentLayer(newCurrentLayer);
    } else
    {
        qDebug() << Q_FUNC_INFO << "layer with name doesn't exist:" << name;
        return false;
    }
}

/*! \overload

  Sets the provided \a layer to be the current layer.

  Returns true on success, i.e. when \a layer is a valid layer in the PF_GraphicView.

  \see addLayer, moveLayer, removeLayer
*/
bool PF_GraphicView::setCurrentLayer(QCPLayer *layer)
{
    if (!mLayers.contains(layer))
    {
        qDebug() << Q_FUNC_INFO << "layer not a layer of this PF_GraphicView:" << reinterpret_cast<quintptr>(layer);
        return false;
    }

    mCurrentLayer = layer;
    return true;
}

/*!
  Returns the number of currently existing layers in the plot

  \see layer, addLayer
*/
int PF_GraphicView::layerCount() const
{
    return mLayers.size();
}

/*!
  Adds a new layer to this PF_GraphicView instance. The new layer will have the name \a name, which
  must be unique. Depending on \a insertMode, it is positioned either below or above \a otherLayer.

  Returns true on success, i.e. if there is no other layer named \a name and \a otherLayer is a
  valid layer inside this PF_GraphicView.

  If \a otherLayer is 0, the highest layer in the PF_GraphicView will be used.

  For an explanation of what layers are in PF_GraphicView, see the documentation of \ref QCPLayer.

  \see layer, moveLayer, removeLayer
*/
bool PF_GraphicView::addLayer(const QString &name, QCPLayer *otherLayer, PF_GraphicView::LayerInsertMode insertMode)
{
    if (!otherLayer)
        otherLayer = mLayers.last();
    if (!mLayers.contains(otherLayer))
    {
        qDebug() << Q_FUNC_INFO << "otherLayer not a layer of this PF_GraphicView:" << reinterpret_cast<quintptr>(otherLayer);
        return false;
    }
    if (layer(name))
    {
        qDebug() << Q_FUNC_INFO << "A layer exists already with the name" << name;
        return false;
    }

    QCPLayer *newLayer = new QCPLayer(this, name);
    mLayers.insert(otherLayer->index() + (insertMode==limAbove ? 1:0), newLayer);
    updateLayerIndices();
    setupPaintBuffers(); // associates new layer with the appropriate paint buffer
    return true;
}

/*!
  Removes the specified \a layer and returns true on success.

  All layerables (e.g. plottables and items) on the removed layer will be moved to the layer below
  \a layer. If \a layer is the bottom layer, the layerables are moved to the layer above. In both
  cases, the total rendering order of all layerables in the PF_GraphicView is preserved.

  If \a layer is the current layer (\ref setCurrentLayer), the layer below (or above, if bottom
  layer) becomes the new current layer.

  It is not possible to remove the last layer of the plot.

  \see layer, addLayer, moveLayer
*/
bool PF_GraphicView::removeLayer(QCPLayer *layer)
{
    if (!mLayers.contains(layer))
    {
        qDebug() << Q_FUNC_INFO << "layer not a layer of this PF_GraphicView:" << reinterpret_cast<quintptr>(layer);
        return false;
    }
    if (mLayers.size() < 2)
    {
        qDebug() << Q_FUNC_INFO << "can't remove last layer";
        return false;
    }

    // append all children of this layer to layer below (if this is lowest layer, prepend to layer above)
    int removedIndex = layer->index();
    bool isFirstLayer = removedIndex==0;
    QCPLayer *targetLayer = isFirstLayer ? mLayers.at(removedIndex+1) : mLayers.at(removedIndex-1);
    QList<QCPLayerable*> children = layer->children();
    if (isFirstLayer) // prepend in reverse order (so order relative to each other stays the same)
    {
        for (int i=children.size()-1; i>=0; --i)
            children.at(i)->moveToLayer(targetLayer, true);
    } else  // append normally
    {
        for (int i=0; i<children.size(); ++i)
            children.at(i)->moveToLayer(targetLayer, false);
    }
    // if removed layer is current layer, change current layer to layer below/above:
    if (layer == mCurrentLayer)
        setCurrentLayer(targetLayer);
    // invalidate the paint buffer that was responsible for this layer:
    if (!layer->mPaintBuffer.isNull())
        layer->mPaintBuffer.data()->setInvalidated();
    // remove layer:
    delete layer;
    mLayers.removeOne(layer);
    updateLayerIndices();
    return true;
}

/*!
  Moves the specified \a layer either above or below \a otherLayer. Whether it's placed above or
  below is controlled with \a insertMode.

  Returns true on success, i.e. when both \a layer and \a otherLayer are valid layers in the
  PF_GraphicView.

  \see layer, addLayer, moveLayer
*/
bool PF_GraphicView::moveLayer(QCPLayer *layer, QCPLayer *otherLayer, PF_GraphicView::LayerInsertMode insertMode)
{
    if (!mLayers.contains(layer))
    {
        qDebug() << Q_FUNC_INFO << "layer not a layer of this PF_GraphicView:" << reinterpret_cast<quintptr>(layer);
        return false;
    }
    if (!mLayers.contains(otherLayer))
    {
        qDebug() << Q_FUNC_INFO << "otherLayer not a layer of this PF_GraphicView:" << reinterpret_cast<quintptr>(otherLayer);
        return false;
    }

    if (layer->index() > otherLayer->index())
        mLayers.move(layer->index(), otherLayer->index() + (insertMode==limAbove ? 1:0));
    else if (layer->index() < otherLayer->index())
        mLayers.move(layer->index(), otherLayer->index() + (insertMode==limAbove ? 0:-1));

    // invalidate the paint buffers that are responsible for the layers:
    if (!layer->mPaintBuffer.isNull())
        layer->mPaintBuffer.data()->setInvalidated();
    if (!otherLayer->mPaintBuffer.isNull())
        otherLayer->mPaintBuffer.data()->setInvalidated();

    updateLayerIndices();
    return true;
}

/*!
  Returns the number of axis rects in the plot.

  All axis rects can be accessed via PF_GraphicView::axisRect().

  Initially, only one axis rect exists in the plot.

  \see axisRect, axisRects
*/
int PF_GraphicView::axisRectCount() const
{
    return axisRects().size();
}

/*!
  Returns the axis rect with \a index.

  Initially, only one axis rect (with index 0) exists in the plot. If multiple axis rects were
  added, all of them may be accessed with this function in a linear fashion (even when they are
  nested in a layout hierarchy or inside other axis rects via QCPAxisRect::insetLayout).

  The order of the axis rects is given by the fill order of the \ref QCPLayout that is holding
  them. For example, if the axis rects are in the top level grid layout (accessible via \ref
  PF_GraphicView::plotLayout), they are ordered from left to right, top to bottom, if the layout's
  default \ref QCPLayoutGrid::setFillOrder "setFillOrder" of \ref QCPLayoutGrid::foColumnsFirst
  "foColumnsFirst" wasn't changed.

  If you want to access axis rects by their row and column index, use the layout interface. For
  example, use \ref QCPLayoutGrid::element of the top level grid layout, and \c qobject_cast the
  returned layout element to \ref QCPAxisRect. (See also \ref thelayoutsystem.)

  \see axisRectCount, axisRects, QCPLayoutGrid::setFillOrder
*/
QCPAxisRect *PF_GraphicView::axisRect(int index) const
{
    const QList<QCPAxisRect*> rectList = axisRects();
    if (index >= 0 && index < rectList.size())
    {
        return rectList.at(index);
    } else
    {
        qDebug() << Q_FUNC_INFO << "invalid axis rect index" << index;
        return nullptr;
    }
}

/*!
  Returns all axis rects in the plot.

  The order of the axis rects is given by the fill order of the \ref QCPLayout that is holding
  them. For example, if the axis rects are in the top level grid layout (accessible via \ref
  PF_GraphicView::plotLayout), they are ordered from left to right, top to bottom, if the layout's
  default \ref QCPLayoutGrid::setFillOrder "setFillOrder" of \ref QCPLayoutGrid::foColumnsFirst
  "foColumnsFirst" wasn't changed.

  \see axisRectCount, axisRect, QCPLayoutGrid::setFillOrder
*/
QList<QCPAxisRect*> PF_GraphicView::axisRects() const
{
    QList<QCPAxisRect*> result;
    QStack<QCPLayoutElement*> elementStack;
    if (mPlotLayout)
        elementStack.push(mPlotLayout);

    while (!elementStack.isEmpty())
    {
        foreach (QCPLayoutElement *element, elementStack.pop()->elements(false))
        {
            if (element)
            {
                elementStack.push(element);
                if (QCPAxisRect *ar = qobject_cast<QCPAxisRect*>(element))
                    result.append(ar);
            }
        }
    }

    return result;
}

/*!
  Returns the layout element at pixel position \a pos. If there is no element at that position,
  returns 0.

  Only visible elements are used. If \ref QCPLayoutElement::setVisible on the element itself or on
  any of its parent elements is set to false, it will not be considered.

  \see itemAt, plottableAt
*/
QCPLayoutElement *PF_GraphicView::layoutElementAt(const QPointF &pos) const
{
    QCPLayoutElement *currentElement = mPlotLayout;
    bool searchSubElements = true;
    while (searchSubElements && currentElement)
    {
        searchSubElements = false;
        foreach (QCPLayoutElement *subElement, currentElement->elements(false))
        {
            if (subElement && subElement->realVisibility() && subElement->selectTest(pos, false) >= 0)
            {
                currentElement = subElement;
                searchSubElements = true;
                break;
            }
        }
    }
    return currentElement;
}

/*!
  Returns the layout element of type \ref QCPAxisRect at pixel position \a pos. This method ignores
  other layout elements even if they are visually in front of the axis rect (e.g. a \ref
  QCPLegend). If there is no axis rect at that position, returns 0.

  Only visible axis rects are used. If \ref QCPLayoutElement::setVisible on the axis rect itself or
  on any of its parent elements is set to false, it will not be considered.

  \see layoutElementAt
*/
QCPAxisRect *PF_GraphicView::axisRectAt(const QPointF &pos) const
{
    QCPAxisRect *result = nullptr;
    QCPLayoutElement *currentElement = mPlotLayout;
    bool searchSubElements = true;
    while (searchSubElements && currentElement)
    {
        searchSubElements = false;
        foreach (QCPLayoutElement *subElement, currentElement->elements(false))
        {
            if (subElement && subElement->realVisibility() && subElement->selectTest(pos, false) >= 0)
            {
                currentElement = subElement;
                searchSubElements = true;
                if (QCPAxisRect *ar = qobject_cast<QCPAxisRect*>(currentElement))
                    result = ar;
                break;
            }
        }
    }
    return result;
}

/*!
  Returns the axes that currently have selected parts, i.e. whose selection state is not \ref
  QCPAxis::spNone.

  \see selectedPlottables, selectedLegends, setInteractions, QCPAxis::setSelectedParts,
  QCPAxis::setSelectableParts
*/
QList<QCPAxis*> PF_GraphicView::selectedAxes() const
{
    QList<QCPAxis*> result, allAxes;
    foreach (QCPAxisRect *rect, axisRects())
        allAxes << rect->axes();

    foreach (QCPAxis *axis, allAxes)
    {
        if (axis->selectedParts() != QCPAxis::spNone)
            result.append(axis);
    }

    return result;
}

/*!
  Returns the legends that currently have selected parts, i.e. whose selection state is not \ref
  QCPLegend::spNone.

  \see selectedPlottables, selectedAxes, setInteractions, QCPLegend::setSelectedParts,
  QCPLegend::setSelectableParts, QCPLegend::selectedItems
*/
QList<QCPLegend*> PF_GraphicView::selectedLegends() const
{
    QList<QCPLegend*> result;

    QStack<QCPLayoutElement*> elementStack;
    if (mPlotLayout)
        elementStack.push(mPlotLayout);

    while (!elementStack.isEmpty())
    {
        foreach (QCPLayoutElement *subElement, elementStack.pop()->elements(false))
        {
            if (subElement)
            {
                elementStack.push(subElement);
                if (QCPLegend *leg = qobject_cast<QCPLegend*>(subElement))
                {
                    if (leg->selectedParts() != QCPLegend::spNone)
                        result.append(leg);
                }
            }
        }
    }

    return result;
}

/*!
  Deselects all layerables (plottables, items, axes, legends,...) of the PF_GraphicView.

  Since calling this function is not a user interaction, this does not emit the \ref
  selectionChangedByUser signal. The individual selectionChanged signals are emitted though, if the
  objects were previously selected.

  \see setInteractions, selectedPlottables, selectedItems, selectedAxes, selectedLegends
*/
void PF_GraphicView::deselectAll()
{
    foreach (QCPLayer *layer, mLayers)
    {
        foreach (QCPLayerable *layerable, layer->children())
            layerable->deselectEvent(nullptr);
    }
}


/*!
 \brief 将一个实际的向量转换为屏幕上的向量

 \param v
 \return PF_Vector
*/
PF_Vector PF_GraphicView::toGui(PF_Vector v) const
{
    return PF_Vector(toGuiX(v.x), toGuiY(v.y));
}


/*!
 \brief 将一个实际的水平位置转换为屏幕上的位置

 \param x
 \return double
*/
double PF_GraphicView::toGuiX(double x) const
{
    return xAxis->coordToPixel(x);
}


/*!
 \brief 将一个实际的竖直位置转换为屏幕上的位置

 \param y
 \return double
*/
double PF_GraphicView::toGuiY(double y) const
{
    return yAxis->coordToPixel(y);
}


/*!
 \brief 将一个实际的水平距离转换为屏幕上的水平距离

 \param d
 \return double
*/
double PF_GraphicView::toGuiDX(double d) const
{
    double factor = double(xAxis->axisRect()->width()) / double(xAxis->range().size());
    return d*factor;
}


/*!
 \brief 讲一个实际的竖直距离转换为屏幕上的竖直距离

 \param d
 \return double
*/
double PF_GraphicView::toGuiDY(double d) const
{
    double factor = double(yAxis->axisRect()->height()) / double(yAxis->range().size());
    return d*factor;
}


/*!
 \brief 将一个屏幕上的向量转换为实际坐标系内的向量

 \param v
 \return PF_Vector
*/
PF_Vector PF_GraphicView::toGraph(PF_Vector v) const
{
    return PF_Vector(toGraphX(v.x),toGraphY(v.y));
}


/*!
 \brief 将一个屏幕上的位置转换为实际的向量

 \param x
 \param y
 \return PF_Vector
*/
PF_Vector PF_GraphicView::toGraph(int x, int y) const
{
    return PF_Vector(toGraphX(x), toGraphY(y));
}


/*!
 \brief 将一个屏幕上的水平位置转换为实际的x坐标

 \param x
 \return double
*/
double PF_GraphicView::toGraphX(int x) const
{
    return xAxis->pixelToCoord(x);
}


/*!
 \brief 将一个屏幕上的竖直位置转换为实际的y坐标

 \param y
 \return double
*/
double PF_GraphicView::toGraphY(int y) const
{
    return yAxis->pixelToCoord(y);
}


/*!
 \brief 将一个屏幕上的水平距离转换为实际坐标系的水平距离

 \param d
 \return double
*/
double PF_GraphicView::toGraphDX(int d) const
{
    double factor = double(xAxis->range().size()) / double(xAxis->axisRect()->width());
    return d*factor;
}


/*!
 \brief 将一个屏幕上的竖直距离转换为实际坐标系的竖直距离

 \param d
 \return double
*/
double PF_GraphicView::toGraphDY(int d) const
{
    double factor = double(yAxis->range().size()) / double(xAxis->axisRect()->height());
    return d*factor;
}

/*!
 \brief 通过控制坐标轴来实现放大

 \param f
 \param center
*/
void PF_GraphicView::zoomIn(double f, const PF_Vector& center)
{
    if(f < 1e-6){
        qDebug()<<Q_FUNC_INFO<<"Invalid zoom factor";
        return;
    }

    zoomWindow(1/f,center);
}

void PF_GraphicView::zoomOut(double f, const PF_Vector& center)
{
    if(f < 1e-6) {
        qDebug()<<Q_FUNC_INFO<<"Invalid zoom factor";
        return;
    }
    zoomWindow(f,center);
}

/**
 * @brief 根据图形的宽和高，缩放至占满窗口大小。
 *
 * @param axis
 * @param keepAspectRatio
 */
void PF_GraphicView::zoomAuto(bool axis, bool keepAspectRatio)
{
    if(container){
        container->calculateBorders();

        double sx,sy;
        sx = container->getSize().x;
        sy = container->getSize().y;

        /** 判断在xy哪个方向上进行缩放  **/
        if(sx > PF_TOLERANCE && sy > PF_TOLERANCE){
            auto const center = (container->getMax()+container->getMin())*0.5;
            double factor = xAxis->range().size()/yAxis->range().size();
            if((sx/xAxis->range().size())>=(sy/yAxis->range().size())){
                qDebug()<<"zoom in x direction";
                xAxis->setRange(QCPRange(center.x-sx*0.505,center.x+sx*0.505));
                yAxis->setRange(QCPRange(center.y-(sx/factor)*0.51,center.y+(sx/factor)*0.51));
            }else{
                qDebug()<<"zoom in y direction";
                yAxis->setRange(QCPRange(center.y-sy*0.505,center.y+sy*0.505));
                xAxis->setRange(QCPRange(center.x-(sy*factor)*0.51,center.x+(sy*factor)*0.51));
            }
        }

        qDebug()<<"sx,"<<sx<<"sy,"<<sy;

    }
    replot();
}

void PF_GraphicView::zoomWindow(double f, const PF_Vector& center)
{
    /** 如果没有指定中心的话，默认为坐标轴中心 **/
    PF_Vector c;
    if(!center.valid){
        c.x = (xAxis->range().upper+xAxis->range().lower)/2.;
        c.y = (yAxis->range().upper+yAxis->range().lower)/2.;
    }
    if(xAxis){
        xAxis->scaleRange(f,c.x);
    }
    if(yAxis){
        yAxis->scaleRange(f,c.y);
    }
    replot();
}

/*!
 \brief 设置默认的捕捉模式

 \param sm
*/
void PF_GraphicView::setDefaultSnapMode(PF_SnapMode sm)
{
    defaultSnapMode = sm;
    /** 目前姑且默认设置一个捕捉模式，所有的都开始，以后再添加设置的接口 **/
    defaultSnapMode.snapCenter = false;
    defaultSnapMode.snapDistance = false;
    defaultSnapMode.snapEndpoint = true;
    defaultSnapMode.snapFree = true;
    defaultSnapMode.snapGrid = true;
    defaultSnapMode.snapMiddle = false;
    defaultSnapMode.snapOnEntity = false;
    /** 将捕捉模式应用到当前活跃的action当中 **/
    if(eventHandler)
        eventHandler->setSnapMode(sm);
}

/*!
 \brief 返回默认的捕捉模式

 \return PF_SnapMode
*/
PF_SnapMode PF_GraphicView::getDefaultSnapMode() const
{
    return defaultSnapMode;
}

/*!
 \brief 给定坐标点mouse，然后返回离该点最近的网格点

 \param mouse
 \return PF_Vector
*/
PF_Vector PF_GraphicView::snapGrid(const PF_Vector& mouse)
{
    if(!mouse.valid)
        return  {};
    /** 快速检查点在坐标轴内 **/
    if(!xAxis->grid()->visible() || !yAxis->grid()->visible())
        return {};
    if(xAxis->mRange.contains(mouse.x) && yAxis->mRange.contains(mouse.y)){
        double dsxmin = PF_MAXDOUBLE;
        double dsymin = PF_MAXDOUBLE;
        double x,y,ds;
        for(int i = 0; i < xAxis->mTickVector.count();++i){
            ds = abs(xAxis->mTickVector.at(i) - mouse.x);
            if(ds < dsxmin){
                x = xAxis->mTickVector.at(i);
                dsxmin = ds;
            }
        }
//        qDebug()<<"Found Grid X:"<<x;
        if(xAxis->grid()->subGridVisible()){
            for(int i = 0; i < xAxis->mSubTickVector.count();++i){
                ds = abs(xAxis->mSubTickVector.at(i) - mouse.x);
                if(ds < dsxmin){
                    x = xAxis->mSubTickVector.at(i);
                    dsxmin = ds;
                }
            }
        }
//        qDebug()<<"Found subGrid X:"<<x;
        for(int i = 0; i < yAxis->mTickVector.count();++i){
            ds = abs(yAxis->mTickVector.at(i) - mouse.y);
            if(ds < dsymin){
                y = yAxis->mTickVector.at(i);
                dsymin = ds;
            }
        }
//        qDebug()<<"Found Grid Y:"<<y;
        if(yAxis->grid()->subGridVisible()){
            for(int i = 0; i < yAxis->mSubTickVector.count();++i){
                ds = abs(yAxis->mSubTickVector.at(i) - mouse.y);
                if(ds < dsymin){
                    y = yAxis->mSubTickVector.at(i);
                    dsymin = ds;
                }
            }

        }
//        qDebug()<<"Found subGrid Y:"<<y;
        return {x,y};
    }
    return {};

}

/*!
  Causes a complete replot into the internal paint buffer(s). Finally, the widget surface is
  refreshed with the new buffer contents. This is the method that must be called to make changes to
  the plot, e.g. on the axis ranges or data points of graphs, visible.

  The parameter \a refreshPriority can be used to fine-tune the timing of the replot. For example
  if your application calls \ref replot very quickly in succession (e.g. multiple independent
  functions change some aspects of the plot and each wants to make sure the change gets replotted),
  it is advisable to set \a refreshPriority to \ref PF_GraphicView::rpQueuedReplot. This way, the
  actual replotting is deferred to the next event loop iteration. Multiple successive calls of \ref
  replot with this priority will only cause a single replot, avoiding redundant replots and
  improving performance.

  Under a few circumstances, PF_GraphicView causes a replot by itself. Those are resize events of the
  PF_GraphicView widget and user interactions (object selection and range dragging/zooming).

  Before the replot happens, the signal \ref beforeReplot is emitted. After the replot, \ref
  afterReplot is emitted. It is safe to mutually connect the replot slot with any of those two
  signals on two PF_GraphicViews to make them replot synchronously, it won't cause an infinite
  recursion.

  If a layer is in mode \ref QCPLayer::lmBuffered (\ref QCPLayer::setMode), it is also possible to
  replot only that specific layer via \ref QCPLayer::replot. See the documentation there for
  details.
*/
void PF_GraphicView::replot(PF_GraphicView::RefreshPriority refreshPriority)
{
    if (refreshPriority == PF_GraphicView::rpQueuedReplot)
    {
        if (!mReplotQueued)
        {
            mReplotQueued = true;
            QTimer::singleShot(0, this, SLOT(replot()));
        }
        return;
    }

    if (mReplotting) // incase signals loop back to replot slot
        return;
    mReplotting = true;
    mReplotQueued = false;
    emit beforeReplot();
    /**必须在更新布局之前生成好刻度**/
    xAxis->setScaleRatio(yAxis,1);
    xAxis2->setScaleRatio(yAxis,1);
    updateLayout();

    // draw all layered objects (grid, axes, plottables, items, legend,...) into their buffers:
    /**在这里建立图像缓存**/
    setupPaintBuffers();
    foreach (QCPLayer *layer, mLayers)
        layer->drawToPaintBuffer();
    for (int i=0; i<mPaintBuffers.size(); ++i)
        mPaintBuffers.at(i)->setInvalidated(false);

    if ((refreshPriority == rpRefreshHint && mPlottingHints.testFlag(QCP::phImmediateRefresh)) || refreshPriority==rpImmediateRefresh)
        repaint();
    else
        update();

    emit afterReplot();
    mReplotting = false;
}

/*!
  Rescales the axes such that all plottables (like graphs) in the plot are fully visible.

  if \a onlyVisiblePlottables is set to true, only the plottables that have their visibility set to true
  (QCPLayerable::setVisible), will be used to rescale the axes.

  \see QCPAbstractPlottable::rescaleAxes, QCPAxis::rescale
*/
void PF_GraphicView::rescaleAxes(bool onlyVisiblePlottables)
{
    QList<QCPAxis*> allAxes;
    foreach (QCPAxisRect *rect, axisRects())
        allAxes << rect->axes();

    foreach (QCPAxis *axis, allAxes)
        axis->rescale(onlyVisiblePlottables);
}

/*!
  Saves a PDF with the vectorized plot to the file \a fileName. The axis ratio as well as the scale
  of texts and lines will be derived from the specified \a width and \a height. This means, the
  output will look like the normal on-screen output of a PF_GraphicView widget with the corresponding
  pixel width and height. If either \a width or \a height is zero, the exported image will have the
  same dimensions as the PF_GraphicView widget currently has.

  Setting \a exportPen to \ref QCP::epNoCosmetic allows to disable the use of cosmetic pens when
  drawing to the PDF file. Cosmetic pens are pens with numerical width 0, which are always drawn as
  a one pixel wide line, no matter what zoom factor is set in the PDF-Viewer. For more information
  about cosmetic pens, see the QPainter and QPen documentation.

  The objects of the plot will appear in the current selection state. If you don't want any
  selected objects to be painted in their selected look, deselect everything with \ref deselectAll
  before calling this function.

  Returns true on success.

  \warning
  \li If you plan on editing the exported PDF file with a vector graphics editor like Inkscape, it
  is advised to set \a exportPen to \ref QCP::epNoCosmetic to avoid losing those cosmetic lines
  (which might be quite many, because cosmetic pens are the default for e.g. axes and tick marks).
  \li If calling this function inside the constructor of the parent of the PF_GraphicView widget
  (i.e. the MainWindow constructor, if PF_GraphicView is inside the MainWindow), always provide
  explicit non-zero widths and heights. If you leave \a width or \a height as 0 (default), this
  function uses the current width and height of the PF_GraphicView widget. However, in Qt, these
  aren't defined yet inside the constructor, so you would get an image that has strange
  widths/heights.

  \a pdfCreator and \a pdfTitle may be used to set the according metadata fields in the resulting
  PDF file.

  \note On Android systems, this method does nothing and issues an according qDebug warning
  message. This is also the case if for other reasons the define flag \c QT_NO_PRINTER is set.

  \see savePng, saveBmp, saveJpg, saveRastered
*/
bool PF_GraphicView::savePdf(const QString &fileName, int width, int height, QCP::ExportPen exportPen, const QString &pdfCreator, const QString &pdfTitle)
{
    bool success = false;
#ifdef QT_NO_PRINTER
    Q_UNUSED(fileName)
    Q_UNUSED(exportPen)
    Q_UNUSED(width)
    Q_UNUSED(height)
    Q_UNUSED(pdfCreator)
    Q_UNUSED(pdfTitle)
    qDebug() << Q_FUNC_INFO << "Qt was built without printer support (QT_NO_PRINTER). PDF not created.";
#else
    int newWidth, newHeight;
    if (width == 0 || height == 0)
    {
        newWidth = this->width();
        newHeight = this->height();
    } else
    {
        newWidth = width;
        newHeight = height;
    }

    QPrinter printer(QPrinter::ScreenResolution);
    printer.setOutputFileName(fileName);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setColorMode(QPrinter::Color);
    printer.printEngine()->setProperty(QPrintEngine::PPK_Creator, pdfCreator);
    printer.printEngine()->setProperty(QPrintEngine::PPK_DocumentName, pdfTitle);
    QRect oldViewport = viewport();
    setViewport(QRect(0, 0, newWidth, newHeight));
#if QT_VERSION < QT_VERSION_CHECK(5, 3, 0)
    printer.setFullPage(true);
    printer.setPaperSize(viewport().size(), QPrinter::DevicePixel);
#else
    QPageLayout pageLayout;
    pageLayout.setMode(QPageLayout::FullPageMode);
    pageLayout.setOrientation(QPageLayout::Portrait);
    pageLayout.setMargins(QMarginsF(0, 0, 0, 0));
    pageLayout.setPageSize(QPageSize(viewport().size(), QPageSize::Point, QString(), QPageSize::ExactMatch));
    printer.setPageLayout(pageLayout);
#endif
    QCPPainter printpainter;
    if (printpainter.begin(&printer))
    {
        printpainter.setMode(QCPPainter::pmVectorized);
        printpainter.setMode(QCPPainter::pmNoCaching);
        printpainter.setMode(QCPPainter::pmNonCosmetic, exportPen==QCP::epNoCosmetic);
        printpainter.setWindow(mViewport);
        if (mBackgroundBrush.style() != Qt::NoBrush &&
                mBackgroundBrush.color() != Qt::white &&
                mBackgroundBrush.color() != Qt::transparent &&
                mBackgroundBrush.color().alpha() > 0) // draw pdf background color if not white/transparent
            printpainter.fillRect(viewport(), mBackgroundBrush);
        draw(&printpainter);
        printpainter.end();
        success = true;
    }
    setViewport(oldViewport);
#endif // QT_NO_PRINTER
    return success;
}

/*!
  Saves a PNG image file to \a fileName on disc. The output plot will have the dimensions \a width
  and \a height in pixels, multiplied by \a scale. If either \a width or \a height is zero, the
  current width and height of the PF_GraphicView widget is used instead. Line widths and texts etc.
  are not scaled up when larger widths/heights are used. If you want that effect, use the \a scale
  parameter.

  For example, if you set both \a width and \a height to 100 and \a scale to 2, you will end up with an
  image file of size 200*200 in which all graphical elements are scaled up by factor 2 (line widths,
  texts, etc.). This scaling is not done by stretching a 100*100 image, the result will have full
  200*200 pixel resolution.

  If you use a high scaling factor, it is recommended to enable antialiasing for all elements by
  temporarily setting \ref PF_GraphicView::setAntialiasedElements to \ref QCP::aeAll as this allows
  PF_GraphicView to place objects with sub-pixel accuracy.

  image compression can be controlled with the \a quality parameter which must be between 0 and 100
  or -1 to use the default setting.

  The \a resolution will be written to the image file header and has no direct consequence for the
  quality or the pixel size. However, if opening the image with a tool which respects the metadata,
  it will be able to scale the image to match either a given size in real units of length (inch,
  centimeters, etc.), or the target display DPI. You can specify in which units \a resolution is
  given, by setting \a resolutionUnit. The \a resolution is converted to the format's expected
  resolution unit internally.

  Returns true on success. If this function fails, most likely the PNG format isn't supported by
  the system, see Qt docs about QImageWriter::supportedImageFormats().

  The objects of the plot will appear in the current selection state. If you don't want any selected
  objects to be painted in their selected look, deselect everything with \ref deselectAll before calling
  this function.

  If you want the PNG to have a transparent background, call \ref setBackground(const QBrush &brush)
  with no brush (Qt::NoBrush) or a transparent color (Qt::transparent), before saving.

  \warning If calling this function inside the constructor of the parent of the PF_GraphicView widget
  (i.e. the MainWindow constructor, if PF_GraphicView is inside the MainWindow), always provide
  explicit non-zero widths and heights. If you leave \a width or \a height as 0 (default), this
  function uses the current width and height of the PF_GraphicView widget. However, in Qt, these
  aren't defined yet inside the constructor, so you would get an image that has strange
  widths/heights.

  \see savePdf, saveBmp, saveJpg, saveRastered
*/
bool PF_GraphicView::savePng(const QString &fileName, int width, int height, double scale, int quality, int resolution, QCP::ResolutionUnit resolutionUnit)
{
    return saveRastered(fileName, width, height, scale, "PNG", quality, resolution, resolutionUnit);
}

/*!
  Saves a JPEG image file to \a fileName on disc. The output plot will have the dimensions \a width
  and \a height in pixels, multiplied by \a scale. If either \a width or \a height is zero, the
  current width and height of the PF_GraphicView widget is used instead. Line widths and texts etc.
  are not scaled up when larger widths/heights are used. If you want that effect, use the \a scale
  parameter.

  For example, if you set both \a width and \a height to 100 and \a scale to 2, you will end up with an
  image file of size 200*200 in which all graphical elements are scaled up by factor 2 (line widths,
  texts, etc.). This scaling is not done by stretching a 100*100 image, the result will have full
  200*200 pixel resolution.

  If you use a high scaling factor, it is recommended to enable antialiasing for all elements by
  temporarily setting \ref PF_GraphicView::setAntialiasedElements to \ref QCP::aeAll as this allows
  PF_GraphicView to place objects with sub-pixel accuracy.

  image compression can be controlled with the \a quality parameter which must be between 0 and 100
  or -1 to use the default setting.

  The \a resolution will be written to the image file header and has no direct consequence for the
  quality or the pixel size. However, if opening the image with a tool which respects the metadata,
  it will be able to scale the image to match either a given size in real units of length (inch,
  centimeters, etc.), or the target display DPI. You can specify in which units \a resolution is
  given, by setting \a resolutionUnit. The \a resolution is converted to the format's expected
  resolution unit internally.

  Returns true on success. If this function fails, most likely the JPEG format isn't supported by
  the system, see Qt docs about QImageWriter::supportedImageFormats().

  The objects of the plot will appear in the current selection state. If you don't want any selected
  objects to be painted in their selected look, deselect everything with \ref deselectAll before calling
  this function.

  \warning If calling this function inside the constructor of the parent of the PF_GraphicView widget
  (i.e. the MainWindow constructor, if PF_GraphicView is inside the MainWindow), always provide
  explicit non-zero widths and heights. If you leave \a width or \a height as 0 (default), this
  function uses the current width and height of the PF_GraphicView widget. However, in Qt, these
  aren't defined yet inside the constructor, so you would get an image that has strange
  widths/heights.

  \see savePdf, savePng, saveBmp, saveRastered
*/
bool PF_GraphicView::saveJpg(const QString &fileName, int width, int height, double scale, int quality, int resolution, QCP::ResolutionUnit resolutionUnit)
{
    return saveRastered(fileName, width, height, scale, "JPG", quality, resolution, resolutionUnit);
}

/*!
  Saves a BMP image file to \a fileName on disc. The output plot will have the dimensions \a width
  and \a height in pixels, multiplied by \a scale. If either \a width or \a height is zero, the
  current width and height of the PF_GraphicView widget is used instead. Line widths and texts etc.
  are not scaled up when larger widths/heights are used. If you want that effect, use the \a scale
  parameter.

  For example, if you set both \a width and \a height to 100 and \a scale to 2, you will end up with an
  image file of size 200*200 in which all graphical elements are scaled up by factor 2 (line widths,
  texts, etc.). This scaling is not done by stretching a 100*100 image, the result will have full
  200*200 pixel resolution.

  If you use a high scaling factor, it is recommended to enable antialiasing for all elements by
  temporarily setting \ref PF_GraphicView::setAntialiasedElements to \ref QCP::aeAll as this allows
  PF_GraphicView to place objects with sub-pixel accuracy.

  The \a resolution will be written to the image file header and has no direct consequence for the
  quality or the pixel size. However, if opening the image with a tool which respects the metadata,
  it will be able to scale the image to match either a given size in real units of length (inch,
  centimeters, etc.), or the target display DPI. You can specify in which units \a resolution is
  given, by setting \a resolutionUnit. The \a resolution is converted to the format's expected
  resolution unit internally.

  Returns true on success. If this function fails, most likely the BMP format isn't supported by
  the system, see Qt docs about QImageWriter::supportedImageFormats().

  The objects of the plot will appear in the current selection state. If you don't want any selected
  objects to be painted in their selected look, deselect everything with \ref deselectAll before calling
  this function.

  \warning If calling this function inside the constructor of the parent of the PF_GraphicView widget
  (i.e. the MainWindow constructor, if PF_GraphicView is inside the MainWindow), always provide
  explicit non-zero widths and heights. If you leave \a width or \a height as 0 (default), this
  function uses the current width and height of the PF_GraphicView widget. However, in Qt, these
  aren't defined yet inside the constructor, so you would get an image that has strange
  widths/heights.

  \see savePdf, savePng, saveJpg, saveRastered
*/
bool PF_GraphicView::saveBmp(const QString &fileName, int width, int height, double scale, int resolution, QCP::ResolutionUnit resolutionUnit)
{
    return saveRastered(fileName, width, height, scale, "BMP", -1, resolution, resolutionUnit);
}

/*! \internal

  Returns a minimum size hint that corresponds to the minimum size of the top level layout
  (\ref plotLayout). To prevent PF_GraphicView from being collapsed to size/width zero, set a minimum
  size (setMinimumSize) either on the whole PF_GraphicView or on any layout elements inside the plot.
  This is especially important, when placed in a QLayout where other components try to take in as
  much space as possible (e.g. QMdiArea).
*/
QSize PF_GraphicView::minimumSizeHint() const
{
    return mPlotLayout->minimumOuterSizeHint();
}

/*! \internal

  Returns a size hint that is the same as \ref minimumSizeHint.

*/
QSize PF_GraphicView::sizeHint() const
{
    return mPlotLayout->minimumOuterSizeHint();
}

/*! \internal

  Event handler for when the PF_GraphicView widget needs repainting. This does not cause a \ref replot, but
  draws the internal buffer on the widget surface.
*/
void PF_GraphicView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    //xAxis->setScaleRatio(yAxis,1);
    QCPPainter painter(this);
    if (painter.isActive())
    {
        painter.setRenderHint(QPainter::HighQualityAntialiasing); // to make Antialiasing look good if using the OpenGL graphicssystem
        if (mBackgroundBrush.style() != Qt::NoBrush)
            painter.fillRect(mViewport, mBackgroundBrush);
        drawBackground(&painter);
        for (int bufferIndex = 0; bufferIndex < mPaintBuffers.size(); ++bufferIndex)
            mPaintBuffers.at(bufferIndex)->draw(&painter);
    }
}

/*! \internal

  Event handler for a resize of the PF_GraphicView widget. The viewport (which becomes the outer rect
  of mPlotLayout) is resized appropriately. Finally a \ref replot is performed.
*/
void PF_GraphicView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    // resize and repaint the buffer:
    setViewport(rect());
    /**窗口大小改变时，要更新一下布局，防止坐标轴发生变形，否则要在replot当中更新两次**/
    xAxis->setScaleRatio(yAxis,1);
    xAxis2->setScaleRatio(yAxis,1);
    updateLayout();
    replot(rpQueuedRefresh); // queued refresh is important here, to prevent painting issues in some contexts (e.g. MDI subwindow)
}

/*! \internal

 Event handler for when a double click occurs. Emits the \ref mouseDoubleClick signal, then
 determines the layerable under the cursor and forwards the event to it. Finally, emits the
 specialized signals when certain objecs are clicked (e.g. \ref plottableDoubleClick, \ref
 axisDoubleClick, etc.).

 \see mousePressEvent, mouseReleaseEvent
*/
void PF_GraphicView::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit mouseDoubleClick(event);
    mMouseHasMoved = false;
    mMousePressPos = event->pos();

    // determine layerable under the cursor (this event is called instead of the second press event in a double-click):
    QList<QVariant> details;
    QList<QCPLayerable*> candidates = layerableListAt(mMousePressPos, false, &details);
    for (int i=0; i<candidates.size(); ++i)
    {
        event->accept(); // default impl of QCPLayerable's mouse events ignore the event, in that case propagate to next candidate in list
        candidates.at(i)->mouseDoubleClickEvent(event, details.at(i));
        if (event->isAccepted())
        {
            mMouseEventLayerable = candidates.at(i);
            mMouseEventLayerableDetails = details.at(i);
            break;
        }
    }

    // emit specialized object double click signals:
    if (!candidates.isEmpty())
    {
        if (QCPAbstractPlottable *ap = qobject_cast<QCPAbstractPlottable*>(candidates.first()))
        {
            int dataIndex = 0;
            if (!details.first().value<QCPDataSelection>().isEmpty())
                dataIndex = details.first().value<QCPDataSelection>().dataRange().begin();
            emit plottableDoubleClick(ap, dataIndex, event);
        } else if (QCPAxis *ax = qobject_cast<QCPAxis*>(candidates.first()))
            emit axisDoubleClick(ax, details.first().value<QCPAxis::SelectablePart>(), event);
        else if (QCPAbstractItem *ai = qobject_cast<QCPAbstractItem*>(candidates.first()))
            emit itemDoubleClick(ai, event);
        else if (QCPLegend *lg = qobject_cast<QCPLegend*>(candidates.first()))
            emit legendDoubleClick(lg, nullptr, event);
        else if (QCPAbstractLegendItem *li = qobject_cast<QCPAbstractLegendItem*>(candidates.first()))
            emit legendDoubleClick(li->parentLegend(), li, event);
    }

    event->accept(); // in case QCPLayerable reimplementation manipulates event accepted state. In QWidget event system, PF_GraphicView wants to accept the event.
}

/*! \internal

  Event handler for when a mouse button is pressed. Emits the mousePress signal.

  If the current \ref setSelectionRectMode is not \ref QCP::srmNone, passes the event to the
  selection rect. Otherwise determines the layerable under the cursor and forwards the event to it.

  \see mouseMoveEvent, mouseReleaseEvent
*/
void PF_GraphicView::mousePressEvent(QMouseEvent *event)
{
    emit mousePress(event);
    // save some state to tell in releaseEvent whether it was a click:
    mMouseHasMoved = false;
    mMousePressPos = event->pos();

    if (mSelectionRect && mSelectionRectMode != QCP::srmNone)
    {
        if (mSelectionRectMode != QCP::srmZoom || qobject_cast<QCPAxisRect*>(axisRectAt(mMousePressPos))) // in zoom mode only activate selection rect if on an axis rect
            mSelectionRect->startSelection(event);
    } else
    {
        // no selection rect interaction, prepare for click signal emission and forward event to layerable under the cursor:
        QList<QVariant> details;
        QList<QCPLayerable*> candidates = layerableListAt(mMousePressPos, false, &details);
        if (!candidates.isEmpty())
        {
            mMouseSignalLayerable = candidates.first(); // candidate for signal emission is always topmost hit layerable (signal emitted in release event)
            mMouseSignalLayerableDetails = details.first();
        }
        // forward event to topmost candidate which accepts the event:
        for (int i=0; i<candidates.size(); ++i)
        {
            event->accept(); // default impl of QCPLayerable's mouse events call ignore() on the event, in that case propagate to next candidate in list
            candidates.at(i)->mousePressEvent(event, details.at(i));
            if (event->isAccepted())
            {
                mMouseEventLayerable = candidates.at(i);
                mMouseEventLayerableDetails = details.at(i);
                break;
            }
        }
    }

    event->accept(); // in case QCPLayerable reimplementation manipulates event accepted state. In QWidget event system, PF_GraphicView wants to accept the event.

    if(event->button() == Qt::MiddleButton){
        //zoom action
    }
    eventHandler->mousePressEvent(event);
}

/*! \internal

  Event handler for when the cursor is moved. Emits the \ref mouseMove signal.

  If the selection rect (\ref setSelectionRect) is currently active, the event is forwarded to it
  in order to update the rect geometry.

  Otherwise, if a layout element has mouse capture focus (a mousePressEvent happened on top of the
  layout element before), the mouseMoveEvent is forwarded to that element.

  \see mousePressEvent, mouseReleaseEvent
*/
void PF_GraphicView::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMove(event);

    if (!mMouseHasMoved && (mMousePressPos-event->pos()).manhattanLength() > 3)
        mMouseHasMoved = true; // moved too far from mouse press position, don't handle as click on mouse release

    if (mSelectionRect && mSelectionRect->isActive())
        mSelectionRect->moveSelection(event);
    else if (mMouseEventLayerable) // call event of affected layerable:
        mMouseEventLayerable->mouseMoveEvent(event, mMousePressPos);

    event->accept(); // in case QCPLayerable reimplementation manipulates event accepted state. In QWidget event system, PF_GraphicView wants to accept the event.

    eventHandler->mouseMoveEvent(event);
}

/*! \internal

  Event handler for when a mouse button is released. Emits the \ref mouseRelease signal.

  If the mouse was moved less than a certain threshold in any direction since the \ref
  mousePressEvent, it is considered a click which causes the selection mechanism (if activated via
  \ref setInteractions) to possibly change selection states accordingly. Further, specialized mouse
  click signals are emitted (e.g. \ref plottableClick, \ref axisClick, etc.)

  If a layerable is the mouse capturer (a \ref mousePressEvent happened on top of the layerable
  before), the \ref mouseReleaseEvent is forwarded to that element.

  \see mousePressEvent, mouseMoveEvent
*/
void PF_GraphicView::mouseReleaseEvent(QMouseEvent *event)
{
    emit mouseRelease(event);

    if (!mMouseHasMoved) // mouse hasn't moved (much) between press and release, so handle as click
    {
        if (mSelectionRect && mSelectionRect->isActive()) // a simple click shouldn't successfully finish a selection rect, so cancel it here
            mSelectionRect->cancel();
        if (event->button() == Qt::LeftButton)
            processPointSelection(event);

        // emit specialized click signals of PF_GraphicView instance:
        if (QCPAbstractPlottable *ap = qobject_cast<QCPAbstractPlottable*>(mMouseSignalLayerable))
        {
            int dataIndex = 0;
            if (!mMouseSignalLayerableDetails.value<QCPDataSelection>().isEmpty())
                dataIndex = mMouseSignalLayerableDetails.value<QCPDataSelection>().dataRange().begin();
            emit plottableClick(ap, dataIndex, event);
        } else if (QCPAxis *ax = qobject_cast<QCPAxis*>(mMouseSignalLayerable))
            emit axisClick(ax, mMouseSignalLayerableDetails.value<QCPAxis::SelectablePart>(), event);
        else if (QCPAbstractItem *ai = qobject_cast<QCPAbstractItem*>(mMouseSignalLayerable))
            emit itemClick(ai, event);
        else if (QCPLegend *lg = qobject_cast<QCPLegend*>(mMouseSignalLayerable))
            emit legendClick(lg, nullptr, event);
        else if (QCPAbstractLegendItem *li = qobject_cast<QCPAbstractLegendItem*>(mMouseSignalLayerable))
            emit legendClick(li->parentLegend(), li, event);
        mMouseSignalLayerable = nullptr;
    }

    if (mSelectionRect && mSelectionRect->isActive()) // Note: if a click was detected above, the selection rect is canceled there
    {
        // finish selection rect, the appropriate action will be taken via signal-slot connection:
        mSelectionRect->endSelection(event);
    } else
    {
        // call event of affected layerable:
        if (mMouseEventLayerable)
        {
            mMouseEventLayerable->mouseReleaseEvent(event, mMousePressPos);
            mMouseEventLayerable = nullptr;
        }
    }

    if (noAntialiasingOnDrag())
        replot(rpQueuedReplot);

    event->accept(); // in case QCPLayerable reimplementation manipulates event accepted state. In QWidget event system, PF_GraphicView wants to accept the event.

    switch (event->button()) {
    case Qt::RightButton:
        //Ctrl+Right-Click

        //Shift+Right-Click

        //Right-Click menu

        back();
        break;
    default:
        eventHandler->mouseReleaseEvent(event);
        break;
    }
}

/*! \internal

  Event handler for mouse wheel events. First, the \ref mouseWheel signal is emitted. Then
  determines the affected layerable and forwards the event to it.
*/
void PF_GraphicView::wheelEvent(QWheelEvent *event)
{
    emit mouseWheel(event);
    // forward event to layerable under cursor:
    QList<QCPLayerable*> candidates = layerableListAt(event->pos(), false);
    for (int i=0; i<candidates.size(); ++i)
    {
        event->accept(); // default impl of QCPLayerable's mouse events ignore the event, in that case propagate to next candidate in list
        candidates.at(i)->wheelEvent(event);
        if (event->isAccepted())
            break;
    }
    event->accept(); // in case QCPLayerable reimplementation manipulates event accepted state. In QWidget event system, PF_GraphicView wants to accept the event.
}

/*! \internal

  This function draws the entire plot, including background pixmap, with the specified \a painter.
  It does not make use of the paint buffers like \ref replot, so this is the function typically
  used by saving/exporting methods such as \ref savePdf or \ref toPainter.

  Note that it does not fill the background with the background brush (as the user may specify with
  \ref setBackground(const QBrush &brush)), this is up to the respective functions calling this
  method.
*/
void PF_GraphicView::draw(QCPPainter *painter)
{
    updateLayout();

    // draw viewport background pixmap:
    drawBackground(painter);

    // draw all layered objects (grid, axes, plottables, items, legend,...):
    foreach (QCPLayer *layer, mLayers)
        layer->draw(painter);

    /* Debug code to draw all layout element rects
  foreach (QCPLayoutElement* el, findChildren<QCPLayoutElement*>())
  {
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(QColor(0, 0, 0, 100), 0, Qt::DashLine));
    painter->drawRect(el->rect());
    painter->setPen(QPen(QColor(255, 0, 0, 100), 0, Qt::DashLine));
    painter->drawRect(el->outerRect());
  }
  */
}

/*! \internal

  Performs the layout update steps defined by \ref QCPLayoutElement::UpdatePhase, by calling \ref
  QCPLayoutElement::update on the main plot layout.

  Here, the layout elements calculate their positions and margins, and prepare for the following
  draw call.
*/
void PF_GraphicView::updateLayout()
{
    // run through layout phases:
    mPlotLayout->update(QCPLayoutElement::upPreparation);
    mPlotLayout->update(QCPLayoutElement::upMargins);
    mPlotLayout->update(QCPLayoutElement::upLayout);
}

/*! \internal

  Draws the viewport background pixmap of the plot.

  If a pixmap was provided via \ref setBackground, this function buffers the scaled version
  depending on \ref setBackgroundScaled and \ref setBackgroundScaledMode and then draws it inside
  the viewport with the provided \a painter. The scaled version is buffered in
  mScaledBackgroundPixmap to prevent expensive rescaling at every redraw. It is only updated, when
  the axis rect has changed in a way that requires a rescale of the background pixmap (this is
  dependent on the \ref setBackgroundScaledMode), or when a differend axis background pixmap was
  set.

  Note that this function does not draw a fill with the background brush
  (\ref setBackground(const QBrush &brush)) beneath the pixmap.

  \see setBackground, setBackgroundScaled, setBackgroundScaledMode
*/
void PF_GraphicView::drawBackground(QCPPainter *painter)
{
    // Note: background color is handled in individual replot/save functions

    // draw background pixmap (on top of fill, if brush specified):
    if (!mBackgroundPixmap.isNull())
    {
        if (mBackgroundScaled)
        {
            // check whether mScaledBackground needs to be updated:
            QSize scaledSize(mBackgroundPixmap.size());
            scaledSize.scale(mViewport.size(), mBackgroundScaledMode);
            if (mScaledBackgroundPixmap.size() != scaledSize)
                mScaledBackgroundPixmap = mBackgroundPixmap.scaled(mViewport.size(), mBackgroundScaledMode, Qt::SmoothTransformation);
            painter->drawPixmap(mViewport.topLeft(), mScaledBackgroundPixmap, QRect(0, 0, mViewport.width(), mViewport.height()) & mScaledBackgroundPixmap.rect());
        } else
        {
            painter->drawPixmap(mViewport.topLeft(), mBackgroundPixmap, QRect(0, 0, mViewport.width(), mViewport.height()));
        }
    }
}

/*! \internal

  Goes through the layers and makes sure this PF_GraphicView instance holds the correct number of
  paint buffers and that they have the correct configuration (size, pixel ratio, etc.).
  Allocations, reallocations and deletions of paint buffers are performed as necessary. It also
  associates the paint buffers with the layers, so they draw themselves into the right buffer when
  \ref QCPLayer::drawToPaintBuffer is called. This means it associates adjacent \ref
  QCPLayer::lmLogical layers to a mutual paint buffer and creates dedicated paint buffers for
  layers in \ref QCPLayer::lmBuffered mode.

  This method uses \ref createPaintBuffer to create new paint buffers.

  After this method, the paint buffers are empty (filled with \c Qt::transparent) and invalidated
  (so an attempt to replot only a single buffered layer causes a full replot).

  This method is called in every \ref replot call, prior to actually drawing the layers (into their
  associated paint buffer). If the paint buffers don't need changing/reallocating, this method
  basically leaves them alone and thus finishes very fast.
*/
void PF_GraphicView::setupPaintBuffers()
{
    int bufferIndex = 0;
    if (mPaintBuffers.isEmpty())
        mPaintBuffers.append(QSharedPointer<QCPAbstractPaintBuffer>(createPaintBuffer()));

    for (int layerIndex = 0; layerIndex < mLayers.size(); ++layerIndex)
    {
        QCPLayer *layer = mLayers.at(layerIndex);
        if (layer->mode() == QCPLayer::lmLogical)
        {
            layer->mPaintBuffer = mPaintBuffers.at(bufferIndex).toWeakRef();
        } else if (layer->mode() == QCPLayer::lmBuffered)
        {
            ++bufferIndex;
            if (bufferIndex >= mPaintBuffers.size())
                mPaintBuffers.append(QSharedPointer<QCPAbstractPaintBuffer>(createPaintBuffer()));
            layer->mPaintBuffer = mPaintBuffers.at(bufferIndex).toWeakRef();
            if (layerIndex < mLayers.size()-1 && mLayers.at(layerIndex+1)->mode() == QCPLayer::lmLogical) // not last layer, and next one is logical, so prepare another buffer for next layerables
            {
                ++bufferIndex;
                if (bufferIndex >= mPaintBuffers.size())
                    mPaintBuffers.append(QSharedPointer<QCPAbstractPaintBuffer>(createPaintBuffer()));
            }
        }
    }
    // remove unneeded buffers:
    while (mPaintBuffers.size()-1 > bufferIndex)
        mPaintBuffers.removeLast();
    // resize buffers to viewport size and clear contents:
    for (int i=0; i<mPaintBuffers.size(); ++i)
    {
        mPaintBuffers.at(i)->setSize(viewport().size()); // won't do anything if already correct size
        mPaintBuffers.at(i)->clear(Qt::transparent);
        mPaintBuffers.at(i)->setInvalidated();
    }
}

/*! \internal

  This method is used by \ref setupPaintBuffers when it needs to create new paint buffers.

  Depending on the current setting of \ref setOpenGl, and the current Qt version, different
  backends (subclasses of \ref QCPAbstractPaintBuffer) are created, initialized with the proper
  size and device pixel ratio, and returned.
*/
QCPAbstractPaintBuffer *PF_GraphicView::createPaintBuffer()
{
    if (mOpenGl)
    {
#if defined(QCP_OPENGL_FBO)
        return new QCPPaintBufferGlFbo(viewport().size(), mBufferDevicePixelRatio, mGlContext, mGlPaintDevice);
#elif defined(QCP_OPENGL_PBUFFER)
        return new QCPPaintBufferGlPbuffer(viewport().size(), mBufferDevicePixelRatio, mOpenGlMultisamples);
#else
        qDebug() << Q_FUNC_INFO << "OpenGL enabled even though no support for it compiled in, this shouldn't have happened. Falling back to pixmap paint buffer.";
        return new QCPPaintBufferPixmap(viewport().size(), mBufferDevicePixelRatio);
#endif
    } else
        return new QCPPaintBufferPixmap(viewport().size(), mBufferDevicePixelRatio);
}

/*!
  This method returns whether any of the paint buffers held by this PF_GraphicView instance are
  invalidated.

  If any buffer is invalidated, a partial replot (\ref QCPLayer::replot) is not allowed and always
  causes a full replot (\ref PF_GraphicView::replot) of all layers. This is the case when for example
  the layer order has changed, new layers were added, layers were removed, or layer modes were
  changed (\ref QCPLayer::setMode).

  \see QCPAbstractPaintBuffer::setInvalidated
*/
bool PF_GraphicView::hasInvalidatedPaintBuffers()
{
    for (int i=0; i<mPaintBuffers.size(); ++i)
    {
        if (mPaintBuffers.at(i)->invalidated())
            return true;
    }
    return false;
}

/*! \internal

  When \ref setOpenGl is set to true, this method is used to initialize OpenGL (create a context,
  surface, paint device).

  Returns true on success.

  If this method is successful, all paint buffers should be deleted and then reallocated by calling
  \ref setupPaintBuffers, so the OpenGL-based paint buffer subclasses (\ref
  QCPPaintBufferGlPbuffer, \ref QCPPaintBufferGlFbo) are used for subsequent replots.

  \see freeOpenGl
*/
bool PF_GraphicView::setupOpenGl()
{
#ifdef QCP_OPENGL_FBO
    freeOpenGl();
    QSurfaceFormat proposedSurfaceFormat;
    proposedSurfaceFormat.setSamples(mOpenGlMultisamples);
#ifdef QCP_OPENGL_OFFSCREENSURFACE
    QOffscreenSurface *surface = new QOffscreenSurface;
#else
    QWindow *surface = new QWindow;
    surface->setSurfaceType(QSurface::OpenGLSurface);
#endif
    surface->setFormat(proposedSurfaceFormat);
    surface->create();
    mGlSurface = QSharedPointer<QSurface>(surface);
    mGlContext = QSharedPointer<QOpenGLContext>(new QOpenGLContext);
    mGlContext->setFormat(mGlSurface->format());
    if (!mGlContext->create())
    {
        qDebug() << Q_FUNC_INFO << "Failed to create OpenGL context";
        mGlContext.clear();
        mGlSurface.clear();
        return false;
    }
    if (!mGlContext->makeCurrent(mGlSurface.data())) // context needs to be current to create paint device
    {
        qDebug() << Q_FUNC_INFO << "Failed to make opengl context current";
        mGlContext.clear();
        mGlSurface.clear();
        return false;
    }
    if (!QOpenGLFramebufferObject::hasOpenGLFramebufferObjects())
    {
        qDebug() << Q_FUNC_INFO << "OpenGL of this system doesn't support frame buffer objects";
        mGlContext.clear();
        mGlSurface.clear();
        return false;
    }
    mGlPaintDevice = QSharedPointer<QOpenGLPaintDevice>(new QOpenGLPaintDevice);
    return true;
#elif defined(QCP_OPENGL_PBUFFER)
    return QGLFormat::hasOpenGL();
#else
    return false;
#endif
}

/*! \internal

  When \ref setOpenGl is set to false, this method is used to deinitialize OpenGL (releases the
  context and frees resources).

  After OpenGL is disabled, all paint buffers should be deleted and then reallocated by calling
  \ref setupPaintBuffers, so the standard software rendering paint buffer subclass (\ref
  QCPPaintBufferPixmap) is used for subsequent replots.

  \see setupOpenGl
*/
void PF_GraphicView::freeOpenGl()
{
#ifdef QCP_OPENGL_FBO
    mGlPaintDevice.clear();
    mGlContext.clear();
    mGlSurface.clear();
#endif
}

/*! \internal

  This method is used by \ref QCPAxisRect::removeAxis to report removed axes to the PF_GraphicView
  so it may clear its PF_GraphicView::xAxis, yAxis, xAxis2 and yAxis2 members accordingly.
*/
void PF_GraphicView::axisRemoved(QCPAxis *axis)
{
    if (xAxis == axis)
        xAxis = nullptr;
    if (xAxis2 == axis)
        xAxis2 = nullptr;
    if (yAxis == axis)
        yAxis = nullptr;
    if (yAxis2 == axis)
        yAxis2 = nullptr;

    // Note: No need to take care of range drag axes and range zoom axes, because they are stored in smart pointers
}

/*! \internal

  This method is used by the QCPLegend destructor to report legend removal to the PF_GraphicView so
  it may clear its PF_GraphicView::legend member accordingly.
*/
void PF_GraphicView::legendRemoved(QCPLegend *legend)
{
    if (this->legend == legend)
        this->legend = nullptr;
}

/*! \internal

  This slot is connected to the selection rect's \ref QCPSelectionRect::accepted signal when \ref
  setSelectionRectMode is set to \ref QCP::srmSelect.

  First, it determines which axis rect was the origin of the selection rect judging by the starting
  point of the selection. Then it goes through the plottables (\ref QCPAbstractPlottable1D to be
  precise) associated with that axis rect and finds the data points that are in \a rect. It does
  this by querying their \ref QCPAbstractPlottable1D::selectTestRect method.

  Then, the actual selection is done by calling the plottables' \ref
  QCPAbstractPlottable::selectEvent, placing the found selected data points in the \a details
  parameter as <tt>QVariant(\ref QCPDataSelection)</tt>. All plottables that weren't touched by \a
  rect receive a \ref QCPAbstractPlottable::deselectEvent.

  \see processRectZoom
*/
void PF_GraphicView::processRectSelection(QRect rect, QMouseEvent *event)
{
    bool selectionStateChanged = false;

    if (mInteractions.testFlag(QCP::iSelectPlottables))
    {
        QMap<int, QPair<QCPAbstractPlottable*, QCPDataSelection> > potentialSelections; // map key is number of selected data points, so we have selections sorted by size
        QRectF rectF(rect.normalized());
        if (QCPAxisRect *affectedAxisRect = axisRectAt(rectF.topLeft()))
        {
            // determine plottables that were hit by the rect and thus are candidates for selection:
//            foreach (QCPAbstractPlottable *plottable, affectedAxisRect->plottables())
//            {
//                if (QCPPlottableInterface1D *plottableInterface = plottable->interface1D())
//                {
//                    QCPDataSelection dataSel = plottableInterface->selectTestRect(rectF, true);
//                    if (!dataSel.isEmpty())
//                        potentialSelections.insertMulti(dataSel.dataPointCount(), QPair<QCPAbstractPlottable*, QCPDataSelection>(plottable, dataSel));
//                }
//            }

            if (!mInteractions.testFlag(QCP::iMultiSelect))
            {
                // only leave plottable with most selected points in map, since we will only select a single plottable:
                if (!potentialSelections.isEmpty())
                {
                    QMap<int, QPair<QCPAbstractPlottable*, QCPDataSelection> >::iterator it = potentialSelections.begin();
                    while (it != potentialSelections.end()-1) // erase all except last element
                        it = potentialSelections.erase(it);
                }
            }

            bool additive = event->modifiers().testFlag(mMultiSelectModifier);
            // deselect all other layerables if not additive selection:
            if (!additive)
            {
                // emit deselection except to those plottables who will be selected afterwards:
                foreach (QCPLayer *layer, mLayers)
                {
                    foreach (QCPLayerable *layerable, layer->children())
                    {
                        if ((potentialSelections.isEmpty() || potentialSelections.constBegin()->first != layerable) && mInteractions.testFlag(layerable->selectionCategory()))
                        {
                            bool selChanged = false;
                            layerable->deselectEvent(&selChanged);
                            selectionStateChanged |= selChanged;
                        }
                    }
                }
            }

            // go through selections in reverse (largest selection first) and emit select events:
            QMap<int, QPair<QCPAbstractPlottable*, QCPDataSelection> >::const_iterator it = potentialSelections.constEnd();
            while (it != potentialSelections.constBegin())
            {
                --it;
                if (mInteractions.testFlag(it.value().first->selectionCategory()))
                {
                    bool selChanged = false;
                    it.value().first->selectEvent(event, additive, QVariant::fromValue(it.value().second), &selChanged);
                    selectionStateChanged |= selChanged;
                }
            }
        }
    }

    if (selectionStateChanged)
    {
        emit selectionChangedByUser();
        replot(rpQueuedReplot);
    } else if (mSelectionRect)
        mSelectionRect->layer()->replot();
}

/*! \internal

  This slot is connected to the selection rect's \ref QCPSelectionRect::accepted signal when \ref
  setSelectionRectMode is set to \ref QCP::srmZoom.

  It determines which axis rect was the origin of the selection rect judging by the starting point
  of the selection, and then zooms the axes defined via \ref QCPAxisRect::setRangeZoomAxes to the
  provided \a rect (see \ref QCPAxisRect::zoom).

  \see processRectSelection
*/
void PF_GraphicView::processRectZoom(QRect rect, QMouseEvent *event)
{
    Q_UNUSED(event)
    if (QCPAxisRect *axisRect = axisRectAt(rect.topLeft()))
    {
        QList<QCPAxis*> affectedAxes = QList<QCPAxis*>() << axisRect->rangeZoomAxes(Qt::Horizontal) << axisRect->rangeZoomAxes(Qt::Vertical);
        affectedAxes.removeAll(static_cast<QCPAxis*>(nullptr));
        axisRect->zoom(QRectF(rect), affectedAxes);
    }
    replot(rpQueuedReplot); // always replot to make selection rect disappear
}

/*! \internal

  This method is called when a simple left mouse click was detected on the PF_GraphicView surface.

  It first determines the layerable that was hit by the click, and then calls its \ref
  QCPLayerable::selectEvent. All other layerables receive a QCPLayerable::deselectEvent (unless the
  multi-select modifier was pressed, see \ref setMultiSelectModifier).

  In this method the hit layerable is determined a second time using \ref layerableAt (after the
  one in \ref mousePressEvent), because we want \a onlySelectable set to true this time. This
  implies that the mouse event grabber (mMouseEventLayerable) may be a different one from the
  clicked layerable determined here. For example, if a non-selectable layerable is in front of a
  selectable layerable at the click position, the front layerable will receive mouse events but the
  selectable one in the back will receive the \ref QCPLayerable::selectEvent.

  \see processRectSelection, QCPLayerable::selectTest
*/
void PF_GraphicView::processPointSelection(QMouseEvent *event)
{
    QVariant details;
    QCPLayerable *clickedLayerable = layerableAt(event->pos(), true, &details);
    bool selectionStateChanged = false;
    bool additive = mInteractions.testFlag(QCP::iMultiSelect) && event->modifiers().testFlag(mMultiSelectModifier);
    // deselect all other layerables if not additive selection:
    if (!additive)
    {
        foreach (QCPLayer *layer, mLayers)
        {
            foreach (QCPLayerable *layerable, layer->children())
            {
                if (layerable != clickedLayerable && mInteractions.testFlag(layerable->selectionCategory()))
                {
                    bool selChanged = false;
                    layerable->deselectEvent(&selChanged);
                    selectionStateChanged |= selChanged;
                }
            }
        }
    }
    if (clickedLayerable && mInteractions.testFlag(clickedLayerable->selectionCategory()))
    {
        // a layerable was actually clicked, call its selectEvent:
        bool selChanged = false;
        clickedLayerable->selectEvent(event, additive, details, &selChanged);
        selectionStateChanged |= selChanged;
    }
    if (selectionStateChanged)
    {
        emit selectionChangedByUser();
        replot(rpQueuedReplot);
    }
}

/*! \internal

  Registers the specified plottable with this PF_GraphicView and, if \ref setAutoAddPlottableToLegend
  is enabled, adds it to the legend (PF_GraphicView::legend). PF_GraphicView takes ownership of the
  plottable.

  Returns true on success, i.e. when \a plottable isn't already in this plot and the parent plot of
  \a plottable is this PF_GraphicView.

  This method is called automatically in the QCPAbstractPlottable base class constructor.
*/
bool PF_GraphicView::registerPlottable(QCPAbstractPlottable *plottable)
{
    if (mPlottables.contains(plottable))
    {
        qDebug() << Q_FUNC_INFO << "plottable already added to this PF_GraphicView:" << reinterpret_cast<quintptr>(plottable);
        return false;
    }
    if (plottable->parentPlot() != this)
    {
        qDebug() << Q_FUNC_INFO << "plottable not created with this PF_GraphicView as parent:" << reinterpret_cast<quintptr>(plottable);
        return false;
    }

    mPlottables.append(plottable);
    // possibly add plottable to legend:
    if (mAutoAddPlottableToLegend)
        plottable->addToLegend();
    if (!plottable->layer()) // usually the layer is already set in the constructor of the plottable (via QCPLayerable constructor)
        plottable->setLayer(currentLayer());
    return true;
}

/*! \internal

  In order to maintain the simplified graph interface of PF_GraphicView, this method is called by the
  QCPGraph constructor to register itself with this PF_GraphicView's internal graph list. Returns true
  on success, i.e. if \a graph is valid and wasn't already registered with this PF_GraphicView.

  This graph specific registration happens in addition to the call to \ref registerPlottable by the
  QCPAbstractPlottable base class.
*/
bool PF_GraphicView::registerGraph(QCPGraph *graph)
{
    if (!graph)
    {
        qDebug() << Q_FUNC_INFO << "passed graph is zero";
        return false;
    }
    if (mGraphs.contains(graph))
    {
        qDebug() << Q_FUNC_INFO << "graph already registered with this PF_GraphicView";
        return false;
    }

    mGraphs.append(graph);
    return true;
}


/*! \internal

  Registers the specified item with this PF_GraphicView. PF_GraphicView takes ownership of the item.

  Returns true on success, i.e. when \a item wasn't already in the plot and the parent plot of \a
  item is this PF_GraphicView.

  This method is called automatically in the QCPAbstractItem base class constructor.
*/
bool PF_GraphicView::registerItem(QCPAbstractItem *item)
{
    if (mItems.contains(item))
    {
        qDebug() << Q_FUNC_INFO << "item already added to this PF_GraphicView:" << reinterpret_cast<quintptr>(item);
        return false;
    }
    if (item->parentPlot() != this)
    {
        qDebug() << Q_FUNC_INFO << "item not created with this PF_GraphicView as parent:" << reinterpret_cast<quintptr>(item);
        return false;
    }

    mItems.append(item);
    if (!item->layer()) // usually the layer is already set in the constructor of the item (via QCPLayerable constructor)
        item->setLayer(currentLayer());
    return true;
}

/*! \internal

  Assigns all layers their index (QCPLayer::mIndex) in the mLayers list. This method is thus called
  after every operation that changes the layer indices, like layer removal, layer creation, layer
  moving.
*/
void PF_GraphicView::updateLayerIndices() const
{
    for (int i=0; i<mLayers.size(); ++i)
        mLayers.at(i)->mIndex = i;
}

/*! \internal

  Returns the top-most layerable at pixel position \a pos. If \a onlySelectable is set to true,
  only those layerables that are selectable will be considered. (Layerable subclasses communicate
  their selectability via the QCPLayerable::selectTest method, by returning -1.)

  \a selectionDetails is an output parameter that contains selection specifics of the affected
  layerable. This is useful if the respective layerable shall be given a subsequent
  QCPLayerable::selectEvent (like in \ref mouseReleaseEvent). \a selectionDetails usually contains
  information about which part of the layerable was hit, in multi-part layerables (e.g.
  QCPAxis::SelectablePart). If the layerable is a plottable, \a selectionDetails contains a \ref
  QCPDataSelection instance with the single data point which is closest to \a pos.

  \see layerableListAt, layoutElementAt, axisRectAt
*/
QCPLayerable *PF_GraphicView::layerableAt(const QPointF &pos, bool onlySelectable, QVariant *selectionDetails) const
{
    QList<QVariant> details;
    QList<QCPLayerable*> candidates = layerableListAt(pos, onlySelectable, selectionDetails ? &details : nullptr);
    if (selectionDetails && !details.isEmpty())
        *selectionDetails = details.first();
    if (!candidates.isEmpty())
        return candidates.first();
    else
        return nullptr;
}

/*! \internal

  Returns the layerables at pixel position \a pos. If \a onlySelectable is set to true, only those
  layerables that are selectable will be considered. (Layerable subclasses communicate their
  selectability via the QCPLayerable::selectTest method, by returning -1.)

  The returned list is sorted by the layerable/drawing order. If you only need to know the top-most
  layerable, rather use \ref layerableAt.

  \a selectionDetails is an output parameter that contains selection specifics of the affected
  layerable. This is useful if the respective layerable shall be given a subsequent
  QCPLayerable::selectEvent (like in \ref mouseReleaseEvent). \a selectionDetails usually contains
  information about which part of the layerable was hit, in multi-part layerables (e.g.
  QCPAxis::SelectablePart). If the layerable is a plottable, \a selectionDetails contains a \ref
  QCPDataSelection instance with the single data point which is closest to \a pos.

  \see layerableAt, layoutElementAt, axisRectAt
*/
QList<QCPLayerable*> PF_GraphicView::layerableListAt(const QPointF &pos, bool onlySelectable, QList<QVariant> *selectionDetails) const
{
    QList<QCPLayerable*> result;
    for (int layerIndex=mLayers.size()-1; layerIndex>=0; --layerIndex)
    {
        const QList<QCPLayerable*> layerables = mLayers.at(layerIndex)->children();
        for (int i=layerables.size()-1; i>=0; --i)
        {
            if (!layerables.at(i)->realVisibility())
                continue;
            QVariant details;
            double dist = layerables.at(i)->selectTest(pos, onlySelectable, selectionDetails ? &details : nullptr);
            if (dist >= 0 && dist < selectionTolerance())
            {
                result.append(layerables.at(i));
                if (selectionDetails)
                    selectionDetails->append(details);
            }
        }
    }
    return result;
}

/*!
  Saves the plot to a rastered image file \a fileName in the image format \a format. The plot is
  sized to \a width and \a height in pixels and scaled with \a scale. (width 100 and scale 2.0 lead
  to a full resolution file with width 200.) If the \a format supports compression, \a quality may
  be between 0 and 100 to control it.

  Returns true on success. If this function fails, most likely the given \a format isn't supported
  by the system, see Qt docs about QImageWriter::supportedImageFormats().

  The \a resolution will be written to the image file header (if the file format supports this) and
  has no direct consequence for the quality or the pixel size. However, if opening the image with a
  tool which respects the metadata, it will be able to scale the image to match either a given size
  in real units of length (inch, centimeters, etc.), or the target display DPI. You can specify in
  which units \a resolution is given, by setting \a resolutionUnit. The \a resolution is converted
  to the format's expected resolution unit internally.

  \see saveBmp, saveJpg, savePng, savePdf
*/
bool PF_GraphicView::saveRastered(const QString &fileName, int width, int height, double scale, const char *format, int quality, int resolution, QCP::ResolutionUnit resolutionUnit)
{
    QImage buffer = toPixmap(width, height, scale).toImage();

    int dotsPerMeter = 0;
    switch (resolutionUnit)
    {
    case QCP::ruDotsPerMeter: dotsPerMeter = resolution; break;
    case QCP::ruDotsPerCentimeter: dotsPerMeter = resolution*100; break;
    case QCP::ruDotsPerInch: dotsPerMeter = resolution/0.0254; break;
    }
    buffer.setDotsPerMeterX(dotsPerMeter); // this is saved together with some image formats, e.g. PNG, and is relevant when opening image in other tools
    buffer.setDotsPerMeterY(dotsPerMeter); // this is saved together with some image formats, e.g. PNG, and is relevant when opening image in other tools
    if (!buffer.isNull())
        return buffer.save(fileName, format, quality);
    else
        return false;
}

/*!
  Renders the plot to a pixmap and returns it.

  The plot is sized to \a width and \a height in pixels and scaled with \a scale. (width 100 and
  scale 2.0 lead to a full resolution pixmap with width 200.)

  \see toPainter, saveRastered, saveBmp, savePng, saveJpg, savePdf
*/
QPixmap PF_GraphicView::toPixmap(int width, int height, double scale)
{
    // this method is somewhat similar to toPainter. Change something here, and a change in toPainter might be necessary, too.
    int newWidth, newHeight;
    if (width == 0 || height == 0)
    {
        newWidth = this->width();
        newHeight = this->height();
    } else
    {
        newWidth = width;
        newHeight = height;
    }
    int scaledWidth = qRound(scale*newWidth);
    int scaledHeight = qRound(scale*newHeight);

    QPixmap result(scaledWidth, scaledHeight);
    result.fill(mBackgroundBrush.style() == Qt::SolidPattern ? mBackgroundBrush.color() : Qt::transparent); // if using non-solid pattern, make transparent now and draw brush pattern later
    QCPPainter painter;
    painter.begin(&result);
    if (painter.isActive())
    {
        QRect oldViewport = viewport();
        setViewport(QRect(0, 0, newWidth, newHeight));
        painter.setMode(QCPPainter::pmNoCaching);
        if (!qFuzzyCompare(scale, 1.0))
        {
            if (scale > 1.0) // for scale < 1 we always want cosmetic pens where possible, because else lines might disappear for very small scales
                painter.setMode(QCPPainter::pmNonCosmetic);
            painter.scale(scale, scale);
        }
        if (mBackgroundBrush.style() != Qt::SolidPattern && mBackgroundBrush.style() != Qt::NoBrush) // solid fills were done a few lines above with QPixmap::fill
            painter.fillRect(mViewport, mBackgroundBrush);
        draw(&painter);
        setViewport(oldViewport);
        painter.end();
    } else // might happen if pixmap has width or height zero
    {
        qDebug() << Q_FUNC_INFO << "Couldn't activate painter on pixmap";
        return QPixmap();
    }
    return result;
}

/*!
  Renders the plot using the passed \a painter.

  The plot is sized to \a width and \a height in pixels. If the \a painter's scale is not 1.0, the resulting plot will
  appear scaled accordingly.

  \note If you are restricted to using a QPainter (instead of QCPPainter), create a temporary QPicture and open a QCPPainter
  on it. Then call \ref toPainter with this QCPPainter. After ending the paint operation on the picture, draw it with
  the QPainter. This will reproduce the painter actions the QCPPainter took, with a QPainter.

  \see toPixmap
*/
void PF_GraphicView::toPainter(QCPPainter *painter, int width, int height)
{
    // this method is somewhat similar to toPixmap. Change something here, and a change in toPixmap might be necessary, too.
    int newWidth, newHeight;
    if (width == 0 || height == 0)
    {
        newWidth = this->width();
        newHeight = this->height();
    } else
    {
        newWidth = width;
        newHeight = height;
    }

    if (painter->isActive())
    {
        QRect oldViewport = viewport();
        setViewport(QRect(0, 0, newWidth, newHeight));
        painter->setMode(QCPPainter::pmNoCaching);
        if (mBackgroundBrush.style() != Qt::NoBrush) // unlike in toPixmap, we can't do QPixmap::fill for Qt::SolidPattern brush style, so we also draw solid fills with fillRect here
            painter->fillRect(mViewport, mBackgroundBrush);
        draw(painter);
        setViewport(oldViewport);
    } else
        qDebug() << Q_FUNC_INFO << "Passed painter is not active";
}
