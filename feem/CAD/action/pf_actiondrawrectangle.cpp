#include "pf_actiondrawrectangle.h"
#include "pf_polyline.h"

#include "pf_graphicview.h"
#include "pf_line.h"

struct PF_ActionDrawRectangle::Points {
    /**
     * 1st corner.
     */
    PF_Vector corner1;
    /**
     * 2nd corner.
     */
    PF_Vector corner2;
};

PF_ActionDrawRectangle::PF_ActionDrawRectangle(PF_EntityContainer *container, PF_GraphicView *graphicView)
    :PF_ActionPreviewInterface("Draw Rectangle",container,graphicView)
    ,pPoints(new Points{})
{
    actionType = PF::ActionDrawRectangle;
}

PF_ActionDrawRectangle::~PF_ActionDrawRectangle()
{

}

void PF_ActionDrawRectangle::trigger()
{
    PF_ActionPreviewInterface::trigger();

    /**创建一个矩形变量**/
    PF_Polyline* polyline = new PF_Polyline(container,view);

    // create and add rectangle:
    polyline->addVertex(pPoints->corner1);
    //polyline->setLayerToActive();
    //polyline->setPenToActive();

    polyline->addVertex({pPoints->corner2.x, pPoints->corner1.y});
    PF_Line::line_index++;
    polyline->addVertex(pPoints->corner2);
    PF_Line::line_index++;
    polyline->addVertex({pPoints->corner1.x, pPoints->corner2.y});
    PF_Line::line_index++;
    polyline->setClosed(true);
    polyline->endPolyline();
    PF_Line::line_index++;
    container->addEntity(polyline);

    view->replot();
}

void PF_ActionDrawRectangle::mouseMoveEvent(QMouseEvent *e)
{
    PF_Vector mouse = snapPoint(e);
    if (getStatus()==SetCorner2 && pPoints->corner1.valid) {
        pPoints->corner2 = mouse;
        deletePreview();
        preview->addRectangle(pPoints->corner1, pPoints->corner2);
        drawPreview();
    }
}

void PF_ActionDrawRectangle::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button()==Qt::LeftButton) {
        PF_Vector mouse = snapPoint(e);

        switch (getStatus()) {
        case SetCorner1:
            pPoints->corner1 = mouse;
            setStatus(SetCorner2);
            break;

        case SetCorner2:
            pPoints->corner2 = mouse;
            trigger();
            setStatus(SetCorner1);
            break;

        default:
            break;
        }
    } else if (e->button()==Qt::RightButton) {
        deletePreview();
        drawPreview();
        init(getStatus()-1);
    }
}

void PF_ActionDrawRectangle::updateMouseButtonHints()
{

}

void PF_ActionDrawRectangle::updateMouseCursor()
{

}
