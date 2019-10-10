#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H
#include "qcustomplot.h"
#include <QWidget>

class PlotWidget : public QWidget
{
public:
    PlotWidget(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void addPlot(QVector<double> x, QVector<double> y);
    QCustomPlot *mCustomPlot;
    QTableView *mTable;
    enum Action{
        ZOOMIN,
        ZOOMOUT,
        TRANSLATION,
        TRACER,
    };

public slots:
    void slotZoomIn();
    void slotZoomOut();
    void slotTranslation();
    void slotTracer();
    void slotReset();

private:
    void setToolBar();
    void uncheckAllToolBar();

    QStandardItemModel *mModel;
    QList<MYQCPItemTracer*> mTracerList;
    QList<MYQCPItemText*> mTextList;
    QToolBar *mToolBar;
    QAction *mActionZoomIn;
    QAction *mActionZoomOut;
    QAction *mActionTranslation;
    QAction *mActionTracer;
    QAction *mActionReset;
    Action mCurrentAction;
};

#endif // PLOTWIDGET_H
