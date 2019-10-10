/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011-2018 Emanuel Eichhammer                            **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.qcustomplot.com/                          **
**             Date: 25.06.18                                             **
**          Version: 2.0.1                                                **
****************************************************************************/

#ifndef QCUSTOMPLOT_H
#define QCUSTOMPLOT_H

#include <QtCore/qglobal.h>

// some Qt version/configuration dependent macros to include or exclude certain code paths:
#ifdef QCUSTOMPLOT_USE_OPENGL
#  if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#    define MYQCP_OPENGL_PBUFFER
#  else
#    define MYQCP_OPENGL_FBO
#  endif
#  if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
#    define MYQCP_OPENGL_OFFSCREENSURFACE
#  endif
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
#  define MYQCP_DEVICEPIXELRATIO_SUPPORTED
#  if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
#    define MYQCP_DEVICEPIXELRATIO_FLOAT
#  endif
#endif

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSharedPointer>
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QPixmap>
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QMultiMap>
#include <QtCore/QFlags>
#include <QtCore/QDebug>
#include <QtCore/QStack>
#include <QtCore/QCache>
#include <QtCore/QMargins>
#include <qmath.h>
#include <limits>
#include <algorithm>
#ifdef MYQCP_OPENGL_FBO
#  include <QtGui/QOpenGLContext>
#  include <QtGui/QOpenGLFramebufferObject>
#  ifdef MYQCP_OPENGL_OFFSCREENSURFACE
#    include <QtGui/QOffscreenSurface>
#  else
#    include <QtGui/QWindow>
#  endif
#endif
#ifdef MYQCP_OPENGL_PBUFFER
#  include <QtOpenGL/QGLPixelBuffer>
#endif
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#  include <qnumeric.h>
#  include <QtGui/QWidget>
#  include <QtGui/QPrinter>
#  include <QtGui/QPrintEngine>
#else
#  include <QtNumeric>
#  include <QtWidgets/QWidget>
#  include <QtPrintSupport/QtPrintSupport>
#endif

class MYMYQCPPainter;
class QCustomPlot;
class MYQCPLayerable;
class MYQCPLayoutElement;
class MYQCPLayout;
class MYQCPAxis;
class MYQCPAxisRect;
class MYQCPAxisPainterPrivate;
class MYQCPAbstractPlottable;
class MYQCPGraph;
class MYQCPAbstractItem;
class MYQCPPlottableInterface1D;
class MYQCPLegend;
class MYQCPItemPosition;
class MYQCPLayer;
class MYQCPAbstractLegendItem;
class MYQCPSelectionRect;
class MYQCPColorMap;
class MYQCPColorScale;
class MYQCPBars;

/* including file 'src/global.h', size 16357                                 */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

#define QCUSTOMPLOT_VERSION_STR "2.0.1"
#define QCUSTOMPLOT_VERSION 0x020001

// decl definitions for shared library compilation/usage:
#if defined(QT_STATIC_BUILD)
#  define MYQCP_LIB_DECL
#elif defined(QCUSTOMPLOT_COMPILE_LIBRARY)
#  define MYQCP_LIB_DECL Q_DECL_EXPORT
#elif defined(QCUSTOMPLOT_USE_LIBRARY)
#  define MYQCP_LIB_DECL Q_DECL_IMPORT
#else
#  define MYQCP_LIB_DECL
#endif

// define empty macro for Q_DECL_OVERRIDE if it doesn't exist (Qt < 5)
#ifndef Q_DECL_OVERRIDE
#  define Q_DECL_OVERRIDE
#endif

/*!
  The MYQCP Namespace contains general enums, QFlags and functions used throughout the QCustomPlot
  library.
  
  It provides QMetaObject-based reflection of its enums and flags via \a MYQCP::staticMetaObject.
*/
#ifndef Q_MOC_RUN
namespace MYQCP {
#else
class MYQCP { // when in moc-run, make it look like a class, so we get Q_GADGET, Q_ENUMS/Q_FLAGS features in namespace
  Q_GADGET
  Q_ENUMS(ExportPen)
  Q_ENUMS(ResolutionUnit)
  Q_ENUMS(SignDomain)
  Q_ENUMS(MarginSide)
  Q_FLAGS(MarginSides)
  Q_ENUMS(AntialiasedElement)
  Q_FLAGS(AntialiasedElements)
  Q_ENUMS(PlottingHint)
  Q_FLAGS(PlottingHints)
  Q_ENUMS(Interaction)
  Q_FLAGS(Interactions)
  Q_ENUMS(SelectionRectMode)
  Q_ENUMS(SelectionType)
public:
#endif

/*!
  Defines the different units in which the image resolution can be specified in the export
  functions.

  \see QCustomPlot::savePng, QCustomPlot::saveJpg, QCustomPlot::saveBmp, QCustomPlot::saveRastered
*/
enum ResolutionUnit { ruDotsPerMeter       ///< Resolution is given in dots per meter (dpm)
                      ,ruDotsPerCentimeter ///< Resolution is given in dots per centimeter (dpcm)
                      ,ruDotsPerInch       ///< Resolution is given in dots per inch (DPI/PPI)
                    };

/*!
  Defines how cosmetic pens (pens with numerical width 0) are handled during export.

  \see QCustomPlot::savePdf
*/
enum ExportPen { epNoCosmetic     ///< Cosmetic pens are converted to pens with pixel width 1 when exporting
                 ,epAllowCosmetic ///< Cosmetic pens are exported normally (e.g. in PDF exports, cosmetic pens always appear as 1 pixel on screen, independent of viewer zoom level)
               };

/*!
  Represents negative and positive sign domain, e.g. for passing to \ref
  MYQCPAbstractPlottable::getKeyRange and \ref MYQCPAbstractPlottable::getValueRange.
  
  This is primarily needed when working with logarithmic axis scales, since only one of the sign
  domains can be visible at a time.
*/
enum SignDomain { sdNegative  ///< The negative sign domain, i.e. numbers smaller than zero
                  ,sdBoth     ///< Both sign domains, including zero, i.e. all numbers
                  ,sdPositive ///< The positive sign domain, i.e. numbers greater than zero
                };

/*!
  Defines the sides of a rectangular entity to which margins can be applied.
  
  \see MYQCPLayoutElement::setAutoMargins, MYQCPAxisRect::setAutoMargins
*/
enum MarginSide { msLeft     = 0x01 ///< <tt>0x01</tt> left margin
                  ,msRight   = 0x02 ///< <tt>0x02</tt> right margin
                  ,msTop     = 0x04 ///< <tt>0x04</tt> top margin
                  ,msBottom  = 0x08 ///< <tt>0x08</tt> bottom margin
                  ,msAll     = 0xFF ///< <tt>0xFF</tt> all margins
                  ,msNone    = 0x00 ///< <tt>0x00</tt> no margin
                };
Q_DECLARE_FLAGS(MarginSides, MarginSide)

/*!
  Defines what objects of a plot can be forcibly drawn antialiased/not antialiased. If an object is
  neither forcibly drawn antialiased nor forcibly drawn not antialiased, it is up to the respective
  element how it is drawn. Typically it provides a \a setAntialiased function for this.
  
  \c AntialiasedElements is a flag of or-combined elements of this enum type.
  
  \see QCustomPlot::setAntialiasedElements, QCustomPlot::setNotAntialiasedElements
*/
enum AntialiasedElement { aeAxes           = 0x0001 ///< <tt>0x0001</tt> Axis base line and tick marks
                          ,aeGrid          = 0x0002 ///< <tt>0x0002</tt> Grid lines
                          ,aeSubGrid       = 0x0004 ///< <tt>0x0004</tt> Sub grid lines
                          ,aeLegend        = 0x0008 ///< <tt>0x0008</tt> Legend box
                          ,aeLegendItems   = 0x0010 ///< <tt>0x0010</tt> Legend items
                          ,aePlottables    = 0x0020 ///< <tt>0x0020</tt> Main lines of plottables
                          ,aeItems         = 0x0040 ///< <tt>0x0040</tt> Main lines of items
                          ,aeScatters      = 0x0080 ///< <tt>0x0080</tt> Scatter symbols of plottables (excluding scatter symbols of type ssPixmap)
                          ,aeFills         = 0x0100 ///< <tt>0x0100</tt> Borders of fills (e.g. under or between graphs)
                          ,aeZeroLine      = 0x0200 ///< <tt>0x0200</tt> Zero-lines, see \ref MYQCPGrid::setZeroLinePen
                          ,aeOther         = 0x8000 ///< <tt>0x8000</tt> Other elements that don't fit into any of the existing categories
                          ,aeAll           = 0xFFFF ///< <tt>0xFFFF</tt> All elements
                          ,aeNone          = 0x0000 ///< <tt>0x0000</tt> No elements
                        };
Q_DECLARE_FLAGS(AntialiasedElements, AntialiasedElement)

/*!
  Defines plotting hints that control various aspects of the quality and speed of plotting.
  
  \see QCustomPlot::setPlottingHints
*/
enum PlottingHint { phNone              = 0x000 ///< <tt>0x000</tt> No hints are set
                    ,phFastPolylines    = 0x001 ///< <tt>0x001</tt> Graph/Curve lines are drawn with a faster method. This reduces the quality especially of the line segment
                                                ///<                joins, thus is most effective for pen sizes larger than 1. It is only used for solid line pens.
                    ,phImmediateRefresh = 0x002 ///< <tt>0x002</tt> causes an immediate repaint() instead of a soft update() when QCustomPlot::replot() is called with parameter \ref QCustomPlot::rpRefreshHint.
                                                ///<                This is set by default to prevent the plot from freezing on fast consecutive replots (e.g. user drags ranges with mouse).
                    ,phCacheLabels      = 0x004 ///< <tt>0x004</tt> axis (tick) labels will be cached as pixmaps, increasing replot performance.
                  };
Q_DECLARE_FLAGS(PlottingHints, PlottingHint)

/*!
  Defines the mouse interactions possible with QCustomPlot.
  
  \c Interactions is a flag of or-combined elements of this enum type.
  
  \see QCustomPlot::setInteractions
*/
enum Interaction { iRangeDrag         = 0x001 ///< <tt>0x001</tt> Axis ranges are draggable (see \ref MYQCPAxisRect::setRangeDrag, \ref MYQCPAxisRect::setRangeDragAxes)
                   ,iRangeZoom        = 0x002 ///< <tt>0x002</tt> Axis ranges are zoomable with the mouse wheel (see \ref MYQCPAxisRect::setRangeZoom, \ref MYQCPAxisRect::setRangeZoomAxes)
                   ,iMultiSelect      = 0x004 ///< <tt>0x004</tt> The user can select multiple objects by holding the modifier set by \ref QCustomPlot::setMultiSelectModifier while clicking
                   ,iSelectPlottables = 0x008 ///< <tt>0x008</tt> Plottables are selectable (e.g. graphs, curves, bars,... see MYQCPAbstractPlottable)
                   ,iSelectAxes       = 0x010 ///< <tt>0x010</tt> Axes are selectable (or parts of them, see MYQCPAxis::setSelectableParts)
                   ,iSelectLegend     = 0x020 ///< <tt>0x020</tt> Legends are selectable (or their child items, see MYQCPLegend::setSelectableParts)
                   ,iSelectItems      = 0x040 ///< <tt>0x040</tt> Items are selectable (Rectangles, Arrows, Textitems, etc. see \ref MYQCPAbstractItem)
                   ,iSelectOther      = 0x080 ///< <tt>0x080</tt> All other objects are selectable (e.g. your own derived layerables, other layout elements,...)
                 };
Q_DECLARE_FLAGS(Interactions, Interaction)

/*!
  Defines the behaviour of the selection rect.
  
  \see QCustomPlot::setSelectionRectMode, QCustomPlot::selectionRect, MYQCPSelectionRect
*/
enum SelectionRectMode { srmNone    ///< The selection rect is disabled, and all mouse events are forwarded to the underlying objects, e.g. for axis range dragging
                         ,srmZoom   ///< When dragging the mouse, a selection rect becomes active. Upon releasing, the axes that are currently set as range zoom axes (\ref MYQCPAxisRect::setRangeZoomAxes) will have their ranges zoomed accordingly.
                         ,srmSelect ///< When dragging the mouse, a selection rect becomes active. Upon releasing, plottable data points that were within the selection rect are selected, if the plottable's selectability setting permits. (See  \ref dataselection "data selection mechanism" for details.)
                         ,srmCustom ///< When dragging the mouse, a selection rect becomes active. It is the programmer's responsibility to connect according slots to the selection rect's signals (e.g. \ref MYQCPSelectionRect::accepted) in order to process the user interaction.
                       };

/*!
  Defines the different ways a plottable can be selected. These images show the effect of the
  different selection types, when the indicated selection rect was dragged:
  
  <center>
  <table>
  <tr>
    <td>\image html selectiontype-none.png stNone</td>
    <td>\image html selectiontype-whole.png stWhole</td>
    <td>\image html selectiontype-singledata.png stSingleData</td>
    <td>\image html selectiontype-datarange.png stDataRange</td>
    <td>\image html selectiontype-multipledataranges.png stMultipleDataRanges</td>
  </tr>
  </table>
  </center>
  
  \see MYQCPAbstractPlottable::setSelectable, MYQCPDataSelection::enforceType
*/
enum SelectionType { stNone                ///< The plottable is not selectable
                     ,stWhole              ///< Selection behaves like \ref stMultipleDataRanges, but if there are any data points selected, the entire plottable is drawn as selected.
                     ,stSingleData         ///< One individual data point can be selected at a time
                     ,stDataRange          ///< Multiple contiguous data points (a data range) can be selected
                     ,stMultipleDataRanges ///< Any combination of data points/ranges can be selected
                    };

/*! \internal
  
  Returns whether the specified \a value is considered an invalid data value for plottables (i.e.
  is \e nan or \e +/-inf). This function is used to check data validity upon replots, when the
  compiler flag \c QCUSTOMPLOT_CHECK_DATA is set.
*/
inline bool isInvalidData(double value)
{
  return qIsNaN(value) || qIsInf(value);
}

/*! \internal
  \overload
  
  Checks two arguments instead of one.
*/
inline bool isInvalidData(double value1, double value2)
{
  return isInvalidData(value1) || isInvalidData(value2);
}

/*! \internal
  
  Sets the specified \a side of \a margins to \a value
  
  \see getMarginValue
*/
inline void setMarginValue(QMargins &margins, MYQCP::MarginSide side, int value)
{
  switch (side)
  {
    case MYQCP::msLeft: margins.setLeft(value); break;
    case MYQCP::msRight: margins.setRight(value); break;
    case MYQCP::msTop: margins.setTop(value); break;
    case MYQCP::msBottom: margins.setBottom(value); break;
    case MYQCP::msAll: margins = QMargins(value, value, value, value); break;
    default: break;
  }
}

/*! \internal
  
  Returns the value of the specified \a side of \a margins. If \a side is \ref MYQCP::msNone or
  \ref MYQCP::msAll, returns 0.
  
  \see setMarginValue
*/
inline int getMarginValue(const QMargins &margins, MYQCP::MarginSide side)
{
  switch (side)
  {
    case MYQCP::msLeft: return margins.left();
    case MYQCP::msRight: return margins.right();
    case MYQCP::msTop: return margins.top();
    case MYQCP::msBottom: return margins.bottom();
    default: break;
  }
  return 0;
}


extern const QMetaObject staticMetaObject; // in moc-run we create a static meta object for MYQCP "fake" object. This line is the link to it via MYQCP::staticMetaObject in normal operation as namespace

} // end of namespace MYQCP
Q_DECLARE_OPERATORS_FOR_FLAGS(MYQCP::AntialiasedElements)
Q_DECLARE_OPERATORS_FOR_FLAGS(MYQCP::PlottingHints)
Q_DECLARE_OPERATORS_FOR_FLAGS(MYQCP::MarginSides)
Q_DECLARE_OPERATORS_FOR_FLAGS(MYQCP::Interactions)
Q_DECLARE_METATYPE(MYQCP::ExportPen)
Q_DECLARE_METATYPE(MYQCP::ResolutionUnit)
Q_DECLARE_METATYPE(MYQCP::SignDomain)
Q_DECLARE_METATYPE(MYQCP::MarginSide)
Q_DECLARE_METATYPE(MYQCP::AntialiasedElement)
Q_DECLARE_METATYPE(MYQCP::PlottingHint)
Q_DECLARE_METATYPE(MYQCP::Interaction)
Q_DECLARE_METATYPE(MYQCP::SelectionRectMode)
Q_DECLARE_METATYPE(MYQCP::SelectionType)

/* end of 'src/global.h' */


/* including file 'src/vector2d.h', size 4928                                */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPVector2D
{
public:
  MYQCPVector2D();
  MYQCPVector2D(double x, double y);
  MYQCPVector2D(const QPoint &point);
  MYQCPVector2D(const QPointF &point);
  
  // getters:
  double x() const { return mX; }
  double y() const { return mY; }
  double &rx() { return mX; }
  double &ry() { return mY; }
  
  // setters:
  void setX(double x) { mX = x; }
  void setY(double y) { mY = y; }
  
  // non-virtual methods:
  double length() const { return qSqrt(mX*mX+mY*mY); }
  double lengthSquared() const { return mX*mX+mY*mY; }
  QPoint toPoint() const { return QPoint(mX, mY); }
  QPointF toPointF() const { return QPointF(mX, mY); }
  
  bool isNull() const { return qIsNull(mX) && qIsNull(mY); }
  void normalize();
  MYQCPVector2D normalized() const;
  MYQCPVector2D perpendicular() const { return MYQCPVector2D(-mY, mX); }
  double dot(const MYQCPVector2D &vec) const { return mX*vec.mX+mY*vec.mY; }
  double distanceSquaredToLine(const MYQCPVector2D &start, const MYQCPVector2D &end) const;
  double distanceSquaredToLine(const QLineF &line) const;
  double distanceToStraightLine(const MYQCPVector2D &base, const MYQCPVector2D &direction) const;
  
  MYQCPVector2D &operator*=(double factor);
  MYQCPVector2D &operator/=(double divisor);
  MYQCPVector2D &operator+=(const MYQCPVector2D &vector);
  MYQCPVector2D &operator-=(const MYQCPVector2D &vector);
  
private:
  // property members:
  double mX, mY;
  
  friend inline const MYQCPVector2D operator*(double factor, const MYQCPVector2D &vec);
  friend inline const MYQCPVector2D operator*(const MYQCPVector2D &vec, double factor);
  friend inline const MYQCPVector2D operator/(const MYQCPVector2D &vec, double divisor);
  friend inline const MYQCPVector2D operator+(const MYQCPVector2D &vec1, const MYQCPVector2D &vec2);
  friend inline const MYQCPVector2D operator-(const MYQCPVector2D &vec1, const MYQCPVector2D &vec2);
  friend inline const MYQCPVector2D operator-(const MYQCPVector2D &vec);
};
Q_DECLARE_TYPEINFO(MYQCPVector2D, Q_MOVABLE_TYPE);

inline const MYQCPVector2D operator*(double factor, const MYQCPVector2D &vec) { return MYQCPVector2D(vec.mX*factor, vec.mY*factor); }
inline const MYQCPVector2D operator*(const MYQCPVector2D &vec, double factor) { return MYQCPVector2D(vec.mX*factor, vec.mY*factor); }
inline const MYQCPVector2D operator/(const MYQCPVector2D &vec, double divisor) { return MYQCPVector2D(vec.mX/divisor, vec.mY/divisor); }
inline const MYQCPVector2D operator+(const MYQCPVector2D &vec1, const MYQCPVector2D &vec2) { return MYQCPVector2D(vec1.mX+vec2.mX, vec1.mY+vec2.mY); }
inline const MYQCPVector2D operator-(const MYQCPVector2D &vec1, const MYQCPVector2D &vec2) { return MYQCPVector2D(vec1.mX-vec2.mX, vec1.mY-vec2.mY); }
inline const MYQCPVector2D operator-(const MYQCPVector2D &vec) { return MYQCPVector2D(-vec.mX, -vec.mY); }

/*! \relates MYQCPVector2D

  Prints \a vec in a human readable format to the qDebug output.
*/
inline QDebug operator<< (QDebug d, const MYQCPVector2D &vec)
{
    d.nospace() << "MYQCPVector2D(" << vec.x() << ", " << vec.y() << ")";
    return d.space();
}

/* end of 'src/vector2d.h' */


/* including file 'src/painter.h', size 4035                                 */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPPainter : public QPainter
{
  Q_GADGET
public:
  /*!
    Defines special modes the painter can operate in. They disable or enable certain subsets of features/fixes/workarounds,
    depending on whether they are wanted on the respective output device.
  */
  enum PainterMode { pmDefault       = 0x00   ///< <tt>0x00</tt> Default mode for painting on screen devices
                     ,pmVectorized   = 0x01   ///< <tt>0x01</tt> Mode for vectorized painting (e.g. PDF export). For example, this prevents some antialiasing fixes.
                     ,pmNoCaching    = 0x02   ///< <tt>0x02</tt> Mode for all sorts of exports (e.g. PNG, PDF,...). For example, this prevents using cached pixmap labels
                     ,pmNonCosmetic  = 0x04   ///< <tt>0x04</tt> Turns pen widths 0 to 1, i.e. disables cosmetic pens. (A cosmetic pen is always drawn with width 1 pixel in the vector image/pdf viewer, independent of zoom.)
                   };
  Q_ENUMS(PainterMode)
  Q_FLAGS(PainterModes)
  Q_DECLARE_FLAGS(PainterModes, PainterMode)
  
  MYQCPPainter();
  explicit MYQCPPainter(QPaintDevice *device);
  
  // getters:
  bool antialiasing() const { return testRenderHint(QPainter::Antialiasing); }
  PainterModes modes() const { return mModes; }

  // setters:
  void setAntialiasing(bool enabled);
  void setMode(PainterMode mode, bool enabled=true);
  void setModes(PainterModes modes);

  // methods hiding non-virtual base class functions (QPainter bug workarounds):
  bool begin(QPaintDevice *device);
  void setPen(const QPen &pen);
  void setPen(const QColor &color);
  void setPen(Qt::PenStyle penStyle);
  void drawLine(const QLineF &line);
  void drawLine(const QPointF &p1, const QPointF &p2) {drawLine(QLineF(p1, p2));}
  void save();
  void restore();
  
  // non-virtual methods:
  void makeNonCosmetic();
  
protected:
  // property members:
  PainterModes mModes;
  bool mIsAntialiasing;
  
  // non-property members:
  QStack<bool> mAntialiasingStack;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(MYQCPPainter::PainterModes)
Q_DECLARE_METATYPE(MYQCPPainter::PainterMode)

/* end of 'src/painter.h' */


/* including file 'src/paintbuffer.h', size 4958                             */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPAbstractPaintBuffer
{
public:
  explicit MYQCPAbstractPaintBuffer(const QSize &size, double devicePixelRatio);
  virtual ~MYQCPAbstractPaintBuffer();
  
  // getters:
  QSize size() const { return mSize; }
  bool invalidated() const { return mInvalidated; }
  double devicePixelRatio() const { return mDevicePixelRatio; }
  
  // setters:
  void setSize(const QSize &size);
  void setInvalidated(bool invalidated=true);
  void setDevicePixelRatio(double ratio);
  
  // introduced virtual methods:
  virtual MYQCPPainter *startPainting() = 0;
  virtual void donePainting() {}
  virtual void draw(MYQCPPainter *painter) const = 0;
  virtual void clear(const QColor &color) = 0;
  
protected:
  // property members:
  QSize mSize;
  double mDevicePixelRatio;
  
  // non-property members:
  bool mInvalidated;
  
  // introduced virtual methods:
  virtual void reallocateBuffer() = 0;
};


class MYQCP_LIB_DECL MYQCPPaintBufferPixmap : public MYQCPAbstractPaintBuffer
{
public:
  explicit MYQCPPaintBufferPixmap(const QSize &size, double devicePixelRatio);
  virtual ~MYQCPPaintBufferPixmap();
  
  // reimplemented virtual methods:
  virtual MYQCPPainter *startPainting() Q_DECL_OVERRIDE;
  virtual void draw(MYQCPPainter *painter) const Q_DECL_OVERRIDE;
  void clear(const QColor &color) Q_DECL_OVERRIDE;
  
protected:
  // non-property members:
  QPixmap mBuffer;
  
  // reimplemented virtual methods:
  virtual void reallocateBuffer() Q_DECL_OVERRIDE;
};


#ifdef MYQCP_OPENGL_PBUFFER
class MYQCP_LIB_DECL MYQCPPaintBufferGlPbuffer : public MYQCPAbstractPaintBuffer
{
public:
  explicit MYQCPPaintBufferGlPbuffer(const QSize &size, double devicePixelRatio, int multisamples);
  virtual ~MYQCPPaintBufferGlPbuffer();
  
  // reimplemented virtual methods:
  virtual MYQCPPainter *startPainting() Q_DECL_OVERRIDE;
  virtual void draw(MYQCPPainter *painter) const Q_DECL_OVERRIDE;
  void clear(const QColor &color) Q_DECL_OVERRIDE;
  
protected:
  // non-property members:
  QGLPixelBuffer *mGlPBuffer;
  int mMultisamples;
  
  // reimplemented virtual methods:
  virtual void reallocateBuffer() Q_DECL_OVERRIDE;
};
#endif // MYQCP_OPENGL_PBUFFER


#ifdef MYQCP_OPENGL_FBO
class MYQCP_LIB_DECL MYQCPPaintBufferGlFbo : public MYQCPAbstractPaintBuffer
{
public:
  explicit MYQCPPaintBufferGlFbo(const QSize &size, double devicePixelRatio, QWeakPointer<QOpenGLContext> glContext, QWeakPointer<QOpenGLPaintDevice> glPaintDevice);
  virtual ~MYQCPPaintBufferGlFbo();
  
  // reimplemented virtual methods:
  virtual MYQCPPainter *startPainting() Q_DECL_OVERRIDE;
  virtual void donePainting() Q_DECL_OVERRIDE;
  virtual void draw(MYQCPPainter *painter) const Q_DECL_OVERRIDE;
  void clear(const QColor &color) Q_DECL_OVERRIDE;
  
protected:
  // non-property members:
  QWeakPointer<QOpenGLContext> mGlContext;
  QWeakPointer<QOpenGLPaintDevice> mGlPaintDevice;
  QOpenGLFramebufferObject *mGlFrameBuffer;
  
  // reimplemented virtual methods:
  virtual void reallocateBuffer() Q_DECL_OVERRIDE;
};
#endif // MYQCP_OPENGL_FBO

/* end of 'src/paintbuffer.h' */


/* including file 'src/layer.h', size 6885                                   */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPLayer : public QObject
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QCustomPlot* parentPlot READ parentPlot)
  Q_PROPERTY(QString name READ name)
  Q_PROPERTY(int index READ index)
  Q_PROPERTY(QList<MYQCPLayerable*> children READ children)
  Q_PROPERTY(bool visible READ visible WRITE setVisible)
  Q_PROPERTY(LayerMode mode READ mode WRITE setMode)
  /// \endcond
public:
  
  /*!
    Defines the different rendering modes of a layer. Depending on the mode, certain layers can be
    replotted individually, without the need to replot (possibly complex) layerables on other
    layers.

    \see setMode
  */
  enum LayerMode { lmLogical   ///< Layer is used only for rendering order, and shares paint buffer with all other adjacent logical layers.
                   ,lmBuffered ///< Layer has its own paint buffer and may be replotted individually (see \ref replot).
                 };
  Q_ENUMS(LayerMode)
  
  MYQCPLayer(QCustomPlot* parentPlot, const QString &layerName);
  virtual ~MYQCPLayer();
  
  // getters:
  QCustomPlot *parentPlot() const { return mParentPlot; }
  QString name() const { return mName; }
  int index() const { return mIndex; }
  QList<MYQCPLayerable*> children() const { return mChildren; }
  bool visible() const { return mVisible; }
  LayerMode mode() const { return mMode; }
  
  // setters:
  void setVisible(bool visible);
  void setMode(LayerMode mode);
  
  // non-virtual methods:
  void replot();
  
protected:
  // property members:
  QCustomPlot *mParentPlot;
  QString mName;
  int mIndex;
  QList<MYQCPLayerable*> mChildren;
  bool mVisible;
  LayerMode mMode;
  
  // non-property members:
  QWeakPointer<MYQCPAbstractPaintBuffer> mPaintBuffer;
  
  // non-virtual methods:
  void draw(MYQCPPainter *painter);
  void drawToPaintBuffer();
  void addChild(MYQCPLayerable *layerable, bool prepend);
  void removeChild(MYQCPLayerable *layerable);
  
private:
  Q_DISABLE_COPY(MYQCPLayer)
  
  friend class QCustomPlot;
  friend class MYQCPLayerable;
};
Q_DECLARE_METATYPE(MYQCPLayer::LayerMode)

class MYQCP_LIB_DECL MYQCPLayerable : public QObject
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(bool visible READ visible WRITE setVisible)
  Q_PROPERTY(QCustomPlot* parentPlot READ parentPlot)
  Q_PROPERTY(MYQCPLayerable* parentLayerable READ parentLayerable)
  Q_PROPERTY(MYQCPLayer* layer READ layer WRITE setLayer NOTIFY layerChanged)
  Q_PROPERTY(bool antialiased READ antialiased WRITE setAntialiased)
  /// \endcond
public:
  MYQCPLayerable(QCustomPlot *plot, QString targetLayer=QString(), MYQCPLayerable *parentLayerable=0);
  virtual ~MYQCPLayerable();
  
  // getters:
  bool visible() const { return mVisible; }
  QCustomPlot *parentPlot() const { return mParentPlot; }
  MYQCPLayerable *parentLayerable() const { return mParentLayerable.data(); }
  MYQCPLayer *layer() const { return mLayer; }
  bool antialiased() const { return mAntialiased; }
  
  // setters:
  void setVisible(bool on);
  Q_SLOT bool setLayer(MYQCPLayer *layer);
  bool setLayer(const QString &layerName);
  void setAntialiased(bool enabled);
  
  // introduced virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;

  // non-property methods:
  bool realVisibility() const;
  
signals:
  void layerChanged(MYQCPLayer *newLayer);
  
protected:
  // property members:
  bool mVisible;
  QCustomPlot *mParentPlot;
  QPointer<MYQCPLayerable> mParentLayerable;
  MYQCPLayer *mLayer;
  bool mAntialiased;
  
  // introduced virtual methods:
  virtual void parentPlotInitialized(QCustomPlot *parentPlot);
  virtual MYQCP::Interaction selectionCategory() const;
  virtual QRect clipRect() const;
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const = 0;
  virtual void draw(MYQCPPainter *painter) = 0;
  // selection events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
  virtual void deselectEvent(bool *selectionStateChanged);
  // low-level mouse events:
  virtual void mousePressEvent(QMouseEvent *event, const QVariant &details);
  virtual void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos);
  virtual void mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos);
  virtual void mouseDoubleClickEvent(QMouseEvent *event, const QVariant &details);
  virtual void wheelEvent(QWheelEvent *event);
  
  // non-property methods:
  void initializeParentPlot(QCustomPlot *parentPlot);
  void setParentLayerable(MYQCPLayerable* parentLayerable);
  bool moveToLayer(MYQCPLayer *layer, bool prepend);
  void applyAntialiasingHint(MYQCPPainter *painter, bool localAntialiased, MYQCP::AntialiasedElement overrideElement) const;
  
private:
  Q_DISABLE_COPY(MYQCPLayerable)
  
  friend class QCustomPlot;
  friend class MYQCPLayer;
  friend class MYQCPAxisRect;
};

/* end of 'src/layer.h' */


/* including file 'src/axis/range.h', size 5280                              */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPRange
{
public:
  double lower, upper;
  
  MYQCPRange();
  MYQCPRange(double lower, double upper);
  
  bool operator==(const MYQCPRange& other) const { return lower == other.lower && upper == other.upper; }
  bool operator!=(const MYQCPRange& other) const { return !(*this == other); }
  
  MYQCPRange &operator+=(const double& value) { lower+=value; upper+=value; return *this; }
  MYQCPRange &operator-=(const double& value) { lower-=value; upper-=value; return *this; }
  MYQCPRange &operator*=(const double& value) { lower*=value; upper*=value; return *this; }
  MYQCPRange &operator/=(const double& value) { lower/=value; upper/=value; return *this; }
  friend inline const MYQCPRange operator+(const MYQCPRange&, double);
  friend inline const MYQCPRange operator+(double, const MYQCPRange&);
  friend inline const MYQCPRange operator-(const MYQCPRange& range, double value);
  friend inline const MYQCPRange operator*(const MYQCPRange& range, double value);
  friend inline const MYQCPRange operator*(double value, const MYQCPRange& range);
  friend inline const MYQCPRange operator/(const MYQCPRange& range, double value);
  
  double size() const { return upper-lower; }
  double center() const { return (upper+lower)*0.5; }
  void normalize() { if (lower > upper) qSwap(lower, upper); }
  void expand(const MYQCPRange &otherRange);
  void expand(double includeCoord);
  MYQCPRange expanded(const MYQCPRange &otherRange) const;
  MYQCPRange expanded(double includeCoord) const;
  MYQCPRange bounded(double lowerBound, double upperBound) const;
  MYQCPRange sanitizedForLogScale() const;
  MYQCPRange sanitizedForLinScale() const;
  bool contains(double value) const { return value >= lower && value <= upper; }
  
  static bool validRange(double lower, double upper);
  static bool validRange(const MYQCPRange &range);
  static const double minRange;
  static const double maxRange;
  
};
Q_DECLARE_TYPEINFO(MYQCPRange, Q_MOVABLE_TYPE);

/*! \relates MYQCPRange

  Prints \a range in a human readable format to the qDebug output.
*/
inline QDebug operator<< (QDebug d, const MYQCPRange &range)
{
    d.nospace() << "MYQCPRange(" << range.lower << ", " << range.upper << ")";
    return d.space();
}

/*!
  Adds \a value to both boundaries of the range.
*/
inline const MYQCPRange operator+(const MYQCPRange& range, double value)
{
  MYQCPRange result(range);
  result += value;
  return result;
}

/*!
  Adds \a value to both boundaries of the range.
*/
inline const MYQCPRange operator+(double value, const MYQCPRange& range)
{
  MYQCPRange result(range);
  result += value;
  return result;
}

/*!
  Subtracts \a value from both boundaries of the range.
*/
inline const MYQCPRange operator-(const MYQCPRange& range, double value)
{
  MYQCPRange result(range);
  result -= value;
  return result;
}

/*!
  Multiplies both boundaries of the range by \a value.
*/
inline const MYQCPRange operator*(const MYQCPRange& range, double value)
{
  MYQCPRange result(range);
  result *= value;
  return result;
}

/*!
  Multiplies both boundaries of the range by \a value.
*/
inline const MYQCPRange operator*(double value, const MYQCPRange& range)
{
  MYQCPRange result(range);
  result *= value;
  return result;
}

/*!
  Divides both boundaries of the range by \a value.
*/
inline const MYQCPRange operator/(const MYQCPRange& range, double value)
{
  MYQCPRange result(range);
  result /= value;
  return result;
}

/* end of 'src/axis/range.h' */


/* including file 'src/selection.h', size 8569                               */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPDataRange
{
public:
  MYQCPDataRange();
  MYQCPDataRange(int begin, int end);
  
  bool operator==(const MYQCPDataRange& other) const { return mBegin == other.mBegin && mEnd == other.mEnd; }
  bool operator!=(const MYQCPDataRange& other) const { return !(*this == other); }
  
  // getters:
  int begin() const { return mBegin; }
  int end() const { return mEnd; }
  int size() const { return mEnd-mBegin; }
  int length() const { return size(); }
  
  // setters:
  void setBegin(int begin) { mBegin = begin; }
  void setEnd(int end)  { mEnd = end; }
  
  // non-property methods:
  bool isValid() const { return (mEnd >= mBegin) && (mBegin >= 0); }
  bool isEmpty() const { return length() == 0; }
  MYQCPDataRange bounded(const MYQCPDataRange &other) const;
  MYQCPDataRange expanded(const MYQCPDataRange &other) const;
  MYQCPDataRange intersection(const MYQCPDataRange &other) const;
  MYQCPDataRange adjusted(int changeBegin, int changeEnd) const { return MYQCPDataRange(mBegin+changeBegin, mEnd+changeEnd); }
  bool intersects(const MYQCPDataRange &other) const;
  bool contains(const MYQCPDataRange &other) const;
  
private:
  // property members:
  int mBegin, mEnd;

};
Q_DECLARE_TYPEINFO(MYQCPDataRange, Q_MOVABLE_TYPE);


class MYQCP_LIB_DECL MYQCPDataSelection
{
public:
  explicit MYQCPDataSelection();
  explicit MYQCPDataSelection(const MYQCPDataRange &range);
  
  bool operator==(const MYQCPDataSelection& other) const;
  bool operator!=(const MYQCPDataSelection& other) const { return !(*this == other); }
  MYQCPDataSelection &operator+=(const MYQCPDataSelection& other);
  MYQCPDataSelection &operator+=(const MYQCPDataRange& other);
  MYQCPDataSelection &operator-=(const MYQCPDataSelection& other);
  MYQCPDataSelection &operator-=(const MYQCPDataRange& other);
  friend inline const MYQCPDataSelection operator+(const MYQCPDataSelection& a, const MYQCPDataSelection& b);
  friend inline const MYQCPDataSelection operator+(const MYQCPDataRange& a, const MYQCPDataSelection& b);
  friend inline const MYQCPDataSelection operator+(const MYQCPDataSelection& a, const MYQCPDataRange& b);
  friend inline const MYQCPDataSelection operator+(const MYQCPDataRange& a, const MYQCPDataRange& b);
  friend inline const MYQCPDataSelection operator-(const MYQCPDataSelection& a, const MYQCPDataSelection& b);
  friend inline const MYQCPDataSelection operator-(const MYQCPDataRange& a, const MYQCPDataSelection& b);
  friend inline const MYQCPDataSelection operator-(const MYQCPDataSelection& a, const MYQCPDataRange& b);
  friend inline const MYQCPDataSelection operator-(const MYQCPDataRange& a, const MYQCPDataRange& b);
  
  // getters:
  int dataRangeCount() const { return mDataRanges.size(); }
  int dataPointCount() const;
  MYQCPDataRange dataRange(int index=0) const;
  QList<MYQCPDataRange> dataRanges() const { return mDataRanges; }
  MYQCPDataRange span() const;
  
  // non-property methods:
  void addDataRange(const MYQCPDataRange &dataRange, bool simplify=true);
  void clear();
  bool isEmpty() const { return mDataRanges.isEmpty(); }
  void simplify();
  void enforceType(MYQCP::SelectionType type);
  bool contains(const MYQCPDataSelection &other) const;
  MYQCPDataSelection intersection(const MYQCPDataRange &other) const;
  MYQCPDataSelection intersection(const MYQCPDataSelection &other) const;
  MYQCPDataSelection inverse(const MYQCPDataRange &outerRange) const;
  
private:
  // property members:
  QList<MYQCPDataRange> mDataRanges;
  
  inline static bool lessThanDataRangeBegin(const MYQCPDataRange &a, const MYQCPDataRange &b) { return a.begin() < b.begin(); }
};
Q_DECLARE_METATYPE(MYQCPDataSelection)


/*!
  Return a \ref MYQCPDataSelection with the data points in \a a joined with the data points in \a b.
  The resulting data selection is already simplified (see \ref MYQCPDataSelection::simplify).
*/
inline const MYQCPDataSelection operator+(const MYQCPDataSelection& a, const MYQCPDataSelection& b)
{
  MYQCPDataSelection result(a);
  result += b;
  return result;
}

/*!
  Return a \ref MYQCPDataSelection with the data points in \a a joined with the data points in \a b.
  The resulting data selection is already simplified (see \ref MYQCPDataSelection::simplify).
*/
inline const MYQCPDataSelection operator+(const MYQCPDataRange& a, const MYQCPDataSelection& b)
{
  MYQCPDataSelection result(a);
  result += b;
  return result;
}

/*!
  Return a \ref MYQCPDataSelection with the data points in \a a joined with the data points in \a b.
  The resulting data selection is already simplified (see \ref MYQCPDataSelection::simplify).
*/
inline const MYQCPDataSelection operator+(const MYQCPDataSelection& a, const MYQCPDataRange& b)
{
  MYQCPDataSelection result(a);
  result += b;
  return result;
}

/*!
  Return a \ref MYQCPDataSelection with the data points in \a a joined with the data points in \a b.
  The resulting data selection is already simplified (see \ref MYQCPDataSelection::simplify).
*/
inline const MYQCPDataSelection operator+(const MYQCPDataRange& a, const MYQCPDataRange& b)
{
  MYQCPDataSelection result(a);
  result += b;
  return result;
}

/*!
  Return a \ref MYQCPDataSelection with the data points which are in \a a but not in \a b.
*/
inline const MYQCPDataSelection operator-(const MYQCPDataSelection& a, const MYQCPDataSelection& b)
{
  MYQCPDataSelection result(a);
  result -= b;
  return result;
}

/*!
  Return a \ref MYQCPDataSelection with the data points which are in \a a but not in \a b.
*/
inline const MYQCPDataSelection operator-(const MYQCPDataRange& a, const MYQCPDataSelection& b)
{
  MYQCPDataSelection result(a);
  result -= b;
  return result;
}

/*!
  Return a \ref MYQCPDataSelection with the data points which are in \a a but not in \a b.
*/
inline const MYQCPDataSelection operator-(const MYQCPDataSelection& a, const MYQCPDataRange& b)
{
  MYQCPDataSelection result(a);
  result -= b;
  return result;
}

/*!
  Return a \ref MYQCPDataSelection with the data points which are in \a a but not in \a b.
*/
inline const MYQCPDataSelection operator-(const MYQCPDataRange& a, const MYQCPDataRange& b)
{
  MYQCPDataSelection result(a);
  result -= b;
  return result;
}

/*! \relates MYQCPDataRange

  Prints \a dataRange in a human readable format to the qDebug output.
*/
inline QDebug operator<< (QDebug d, const MYQCPDataRange &dataRange)
{
  d.nospace() << "MYQCPDataRange(" << dataRange.begin() << ", " << dataRange.end() << ")";
  return d;
}

/*! \relates MYQCPDataSelection

  Prints \a selection in a human readable format to the qDebug output.
*/
inline QDebug operator<< (QDebug d, const MYQCPDataSelection &selection)
{
    d.nospace() << "MYQCPDataSelection(";
    for (int i=0; i<selection.dataRangeCount(); ++i)
    {
      if (i != 0)
        d << ", ";
      d << selection.dataRange(i);
    }
    d << ")";
    return d;
}



/* end of 'src/selection.h' */


/* including file 'src/selectionrect.h', size 3338                           */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPSelectionRect : public MYQCPLayerable
{
  Q_OBJECT
public:
  explicit MYQCPSelectionRect(QCustomPlot *parentPlot);
  virtual ~MYQCPSelectionRect();
  
  // getters:
  QRect rect() const { return mRect; }
  MYQCPRange range(const MYQCPAxis *axis) const;
  QPen pen() const { return mPen; }
  QBrush brush() const { return mBrush; }
  bool isActive() const { return mActive; }
  
  // setters:
  void setPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  
  // non-property methods:
  Q_SLOT void cancel();
  
signals:
  void started(QMouseEvent *event);
  void changed(const QRect &rect, QMouseEvent *event);
  void canceled(const QRect &rect, QInputEvent *event);
  void accepted(const QRect &rect, QMouseEvent *event);
  
protected:
  // property members:
  QRect mRect;
  QPen mPen;
  QBrush mBrush;
  // non-property members:
  bool mActive;
  
  // introduced virtual methods:
  virtual void startSelection(QMouseEvent *event);
  virtual void moveSelection(QMouseEvent *event);
  virtual void endSelection(QMouseEvent *event);
  virtual void keyPressEvent(QKeyEvent *event);
  
  // reimplemented virtual methods
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const Q_DECL_OVERRIDE;
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  
  friend class QCustomPlot;
};

/* end of 'src/selectionrect.h' */


/* including file 'src/layout.h', size 14224                                 */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPMarginGroup : public QObject
{
  Q_OBJECT
public:
  explicit MYQCPMarginGroup(QCustomPlot *parentPlot);
  virtual ~MYQCPMarginGroup();
  
  // non-virtual methods:
  QList<MYQCPLayoutElement*> elements(MYQCP::MarginSide side) const { return mChildren.value(side); }
  bool isEmpty() const;
  void clear();
  
protected:
  // non-property members:
  QCustomPlot *mParentPlot;
  QHash<MYQCP::MarginSide, QList<MYQCPLayoutElement*> > mChildren;
  
  // introduced virtual methods:
  virtual int commonMargin(MYQCP::MarginSide side) const;
  
  // non-virtual methods:
  void addChild(MYQCP::MarginSide side, MYQCPLayoutElement *element);
  void removeChild(MYQCP::MarginSide side, MYQCPLayoutElement *element);
  
private:
  Q_DISABLE_COPY(MYQCPMarginGroup)
  
  friend class MYQCPLayoutElement;
};


class MYQCP_LIB_DECL MYQCPLayoutElement : public MYQCPLayerable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(MYQCPLayout* layout READ layout)
  Q_PROPERTY(QRect rect READ rect)
  Q_PROPERTY(QRect outerRect READ outerRect WRITE setOuterRect)
  Q_PROPERTY(QMargins margins READ margins WRITE setMargins)
  Q_PROPERTY(QMargins minimumMargins READ minimumMargins WRITE setMinimumMargins)
  Q_PROPERTY(QSize minimumSize READ minimumSize WRITE setMinimumSize)
  Q_PROPERTY(QSize maximumSize READ maximumSize WRITE setMaximumSize)
  Q_PROPERTY(SizeConstraintRect sizeConstraintRect READ sizeConstraintRect WRITE setSizeConstraintRect)
  /// \endcond
public:
  /*!
    Defines the phases of the update process, that happens just before a replot. At each phase,
    \ref update is called with the according UpdatePhase value.
  */
  enum UpdatePhase { upPreparation ///< Phase used for any type of preparation that needs to be done before margin calculation and layout
                     ,upMargins    ///< Phase in which the margins are calculated and set
                     ,upLayout     ///< Final phase in which the layout system places the rects of the elements
                   };
  Q_ENUMS(UpdatePhase)
  
  /*!
    Defines to which rect of a layout element the size constraints that can be set via \ref
    setMinimumSize and \ref setMaximumSize apply. The outer rect (\ref outerRect) includes the
    margins (e.g. in the case of a MYQCPAxisRect the axis labels), whereas the inner rect (\ref rect)
    does not.
    
    \see setSizeConstraintRect
  */
  enum SizeConstraintRect { scrInnerRect ///< Minimum/Maximum size constraints apply to inner rect
                            , scrOuterRect ///< Minimum/Maximum size constraints apply to outer rect, thus include layout element margins
                          };
  Q_ENUMS(SizeConstraintRect)

  explicit MYQCPLayoutElement(QCustomPlot *parentPlot=0);
  virtual ~MYQCPLayoutElement();
  
  // getters:
  MYQCPLayout *layout() const { return mParentLayout; }
  QRect rect() const { return mRect; }
  QRect outerRect() const { return mOuterRect; }
  QMargins margins() const { return mMargins; }
  QMargins minimumMargins() const { return mMinimumMargins; }
  MYQCP::MarginSides autoMargins() const { return mAutoMargins; }
  QSize minimumSize() const { return mMinimumSize; }
  QSize maximumSize() const { return mMaximumSize; }
  SizeConstraintRect sizeConstraintRect() const { return mSizeConstraintRect; }
  MYQCPMarginGroup *marginGroup(MYQCP::MarginSide side) const { return mMarginGroups.value(side, (MYQCPMarginGroup*)0); }
  QHash<MYQCP::MarginSide, MYQCPMarginGroup*> marginGroups() const { return mMarginGroups; }
  
  // setters:
  void setOuterRect(const QRect &rect);
  void setMargins(const QMargins &margins);
  void setMinimumMargins(const QMargins &margins);
  void setAutoMargins(MYQCP::MarginSides sides);
  void setMinimumSize(const QSize &size);
  void setMinimumSize(int width, int height);
  void setMaximumSize(const QSize &size);
  void setMaximumSize(int width, int height);
  void setSizeConstraintRect(SizeConstraintRect constraintRect);
  void setMarginGroup(MYQCP::MarginSides sides, MYQCPMarginGroup *group);
  
  // introduced virtual methods:
  virtual void update(UpdatePhase phase);
  virtual QSize minimumOuterSizeHint() const;
  virtual QSize maximumOuterSizeHint() const;
  virtual QList<MYQCPLayoutElement*> elements(bool recursive) const;
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  
protected:
  // property members:
  MYQCPLayout *mParentLayout;
  QSize mMinimumSize, mMaximumSize;
  SizeConstraintRect mSizeConstraintRect;
  QRect mRect, mOuterRect;
  QMargins mMargins, mMinimumMargins;
  MYQCP::MarginSides mAutoMargins;
  QHash<MYQCP::MarginSide, MYQCPMarginGroup*> mMarginGroups;
  
  // introduced virtual methods:
  virtual int calculateAutoMargin(MYQCP::MarginSide side);
  virtual void layoutChanged();
  
  // reimplemented virtual methods:
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const Q_DECL_OVERRIDE { Q_UNUSED(painter) }
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE { Q_UNUSED(painter) }
  virtual void parentPlotInitialized(QCustomPlot *parentPlot) Q_DECL_OVERRIDE;

private:
  Q_DISABLE_COPY(MYQCPLayoutElement)
  
  friend class QCustomPlot;
  friend class MYQCPLayout;
  friend class MYQCPMarginGroup;
};
Q_DECLARE_METATYPE(MYQCPLayoutElement::UpdatePhase)


class MYQCP_LIB_DECL MYQCPLayout : public MYQCPLayoutElement
{
  Q_OBJECT
public:
  explicit MYQCPLayout();
  
  // reimplemented virtual methods:
  virtual void update(UpdatePhase phase) Q_DECL_OVERRIDE;
  virtual QList<MYQCPLayoutElement*> elements(bool recursive) const Q_DECL_OVERRIDE;
  
  // introduced virtual methods:
  virtual int elementCount() const = 0;
  virtual MYQCPLayoutElement* elementAt(int index) const = 0;
  virtual MYQCPLayoutElement* takeAt(int index) = 0;
  virtual bool take(MYQCPLayoutElement* element) = 0;
  virtual void simplify();
  
  // non-virtual methods:
  bool removeAt(int index);
  bool remove(MYQCPLayoutElement* element);
  void clear();
  
protected:
  // introduced virtual methods:
  virtual void updateLayout();
  
  // non-virtual methods:
  void sizeConstraintsChanged() const;
  void adoptElement(MYQCPLayoutElement *el);
  void releaseElement(MYQCPLayoutElement *el);
  QVector<int> getSectionSizes(QVector<int> maxSizes, QVector<int> minSizes, QVector<double> stretchFactors, int totalSize) const;
  static QSize getFinalMinimumOuterSize(const MYQCPLayoutElement *el);
  static QSize getFinalMaximumOuterSize(const MYQCPLayoutElement *el);
  
private:
  Q_DISABLE_COPY(MYQCPLayout)
  friend class MYQCPLayoutElement;
};


class MYQCP_LIB_DECL MYQCPLayoutGrid : public MYQCPLayout
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(int rowCount READ rowCount)
  Q_PROPERTY(int columnCount READ columnCount)
  Q_PROPERTY(QList<double> columnStretchFactors READ columnStretchFactors WRITE setColumnStretchFactors)
  Q_PROPERTY(QList<double> rowStretchFactors READ rowStretchFactors WRITE setRowStretchFactors)
  Q_PROPERTY(int columnSpacing READ columnSpacing WRITE setColumnSpacing)
  Q_PROPERTY(int rowSpacing READ rowSpacing WRITE setRowSpacing)
  Q_PROPERTY(FillOrder fillOrder READ fillOrder WRITE setFillOrder)
  Q_PROPERTY(int wrap READ wrap WRITE setWrap)
  /// \endcond
public:
  
  /*!
    Defines in which direction the grid is filled when using \ref addElement(MYQCPLayoutElement*).
    The column/row at which wrapping into the next row/column occurs can be specified with \ref
    setWrap.

    \see setFillOrder
  */
  enum FillOrder { foRowsFirst    ///< Rows are filled first, and a new element is wrapped to the next column if the row count would exceed \ref setWrap.
                  ,foColumnsFirst ///< Columns are filled first, and a new element is wrapped to the next row if the column count would exceed \ref setWrap.
                };
  Q_ENUMS(FillOrder)
  
  explicit MYQCPLayoutGrid();
  virtual ~MYQCPLayoutGrid();
  
  // getters:
  int rowCount() const { return mElements.size(); }
  int columnCount() const { return mElements.size() > 0 ? mElements.first().size() : 0; }
  QList<double> columnStretchFactors() const { return mColumnStretchFactors; }
  QList<double> rowStretchFactors() const { return mRowStretchFactors; }
  int columnSpacing() const { return mColumnSpacing; }
  int rowSpacing() const { return mRowSpacing; }
  int wrap() const { return mWrap; }
  FillOrder fillOrder() const { return mFillOrder; }
  
  // setters:
  void setColumnStretchFactor(int column, double factor);
  void setColumnStretchFactors(const QList<double> &factors);
  void setRowStretchFactor(int row, double factor);
  void setRowStretchFactors(const QList<double> &factors);
  void setColumnSpacing(int pixels);
  void setRowSpacing(int pixels);
  void setWrap(int count);
  void setFillOrder(FillOrder order, bool rearrange=true);
  
  // reimplemented virtual methods:
  virtual void updateLayout() Q_DECL_OVERRIDE;
  virtual int elementCount() const Q_DECL_OVERRIDE { return rowCount()*columnCount(); }
  virtual MYQCPLayoutElement* elementAt(int index) const Q_DECL_OVERRIDE;
  virtual MYQCPLayoutElement* takeAt(int index) Q_DECL_OVERRIDE;
  virtual bool take(MYQCPLayoutElement* element) Q_DECL_OVERRIDE;
  virtual QList<MYQCPLayoutElement*> elements(bool recursive) const Q_DECL_OVERRIDE;
  virtual void simplify() Q_DECL_OVERRIDE;
  virtual QSize minimumOuterSizeHint() const Q_DECL_OVERRIDE;
  virtual QSize maximumOuterSizeHint() const Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  MYQCPLayoutElement *element(int row, int column) const;
  bool addElement(int row, int column, MYQCPLayoutElement *element);
  bool addElement(MYQCPLayoutElement *element);
  bool hasElement(int row, int column);
  void expandTo(int newRowCount, int newColumnCount);
  void insertRow(int newIndex);
  void insertColumn(int newIndex);
  int rowColToIndex(int row, int column) const;
  void indexToRowCol(int index, int &row, int &column) const;
  
protected:
  // property members:
  QList<QList<MYQCPLayoutElement*> > mElements;
  QList<double> mColumnStretchFactors;
  QList<double> mRowStretchFactors;
  int mColumnSpacing, mRowSpacing;
  int mWrap;
  FillOrder mFillOrder;
  
  // non-virtual methods:
  void getMinimumRowColSizes(QVector<int> *minColWidths, QVector<int> *minRowHeights) const;
  void getMaximumRowColSizes(QVector<int> *maxColWidths, QVector<int> *maxRowHeights) const;
  
private:
  Q_DISABLE_COPY(MYQCPLayoutGrid)
};
Q_DECLARE_METATYPE(MYQCPLayoutGrid::FillOrder)


class MYQCP_LIB_DECL MYQCPLayoutInset : public MYQCPLayout
{
  Q_OBJECT
public:
  /*!
    Defines how the placement and sizing is handled for a certain element in a MYQCPLayoutInset.
  */
  enum InsetPlacement { ipFree            ///< The element may be positioned/sized arbitrarily, see \ref setInsetRect
                        ,ipBorderAligned  ///< The element is aligned to one of the layout sides, see \ref setInsetAlignment
                      };
  Q_ENUMS(InsetPlacement)
  
  explicit MYQCPLayoutInset();
  virtual ~MYQCPLayoutInset();
  
  // getters:
  InsetPlacement insetPlacement(int index) const;
  Qt::Alignment insetAlignment(int index) const;
  QRectF insetRect(int index) const;
  
  // setters:
  void setInsetPlacement(int index, InsetPlacement placement);
  void setInsetAlignment(int index, Qt::Alignment alignment);
  void setInsetRect(int index, const QRectF &rect);
  
  // reimplemented virtual methods:
  virtual void updateLayout() Q_DECL_OVERRIDE;
  virtual int elementCount() const Q_DECL_OVERRIDE;
  virtual MYQCPLayoutElement* elementAt(int index) const Q_DECL_OVERRIDE;
  virtual MYQCPLayoutElement* takeAt(int index) Q_DECL_OVERRIDE;
  virtual bool take(MYQCPLayoutElement* element) Q_DECL_OVERRIDE;
  virtual void simplify() Q_DECL_OVERRIDE {}
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  void addElement(MYQCPLayoutElement *element, Qt::Alignment alignment);
  void addElement(MYQCPLayoutElement *element, const QRectF &rect);
  
protected:
  // property members:
  QList<MYQCPLayoutElement*> mElements;
  QList<InsetPlacement> mInsetPlacement;
  QList<Qt::Alignment> mInsetAlignment;
  QList<QRectF> mInsetRect;
  
private:
  Q_DISABLE_COPY(MYQCPLayoutInset)
};
Q_DECLARE_METATYPE(MYQCPLayoutInset::InsetPlacement)

/* end of 'src/layout.h' */


/* including file 'src/lineending.h', size 4426                              */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPLineEnding
{
  Q_GADGET
public:
  /*!
    Defines the type of ending decoration for line-like items, e.g. an arrow.
    
    \image html MYQCPLineEnding.png
    
    The width and length of these decorations can be controlled with the functions \ref setWidth
    and \ref setLength. Some decorations like \ref esDisc, \ref esSquare, \ref esDiamond and \ref esBar only
    support a width, the length property is ignored.
    
    \see MYQCPItemLine::setHead, MYQCPItemLine::setTail, MYQCPItemCurve::setHead, MYQCPItemCurve::setTail, MYQCPAxis::setLowerEnding, MYQCPAxis::setUpperEnding
  */
  enum EndingStyle { esNone          ///< No ending decoration
                     ,esFlatArrow    ///< A filled arrow head with a straight/flat back (a triangle)
                     ,esSpikeArrow   ///< A filled arrow head with an indented back
                     ,esLineArrow    ///< A non-filled arrow head with open back
                     ,esDisc         ///< A filled circle
                     ,esSquare       ///< A filled square
                     ,esDiamond      ///< A filled diamond (45 degrees rotated square)
                     ,esBar          ///< A bar perpendicular to the line
                     ,esHalfBar      ///< A bar perpendicular to the line, pointing out to only one side (to which side can be changed with \ref setInverted)
                     ,esSkewedBar    ///< A bar that is skewed (skew controllable via \ref setLength)
                   };
  Q_ENUMS(EndingStyle)
  
  MYQCPLineEnding();
  MYQCPLineEnding(EndingStyle style, double width=8, double length=10, bool inverted=false);
  
  // getters:
  EndingStyle style() const { return mStyle; }
  double width() const { return mWidth; }
  double length() const { return mLength; }
  bool inverted() const { return mInverted; }
  
  // setters:
  void setStyle(EndingStyle style);
  void setWidth(double width);
  void setLength(double length);
  void setInverted(bool inverted);
  
  // non-property methods:
  double boundingDistance() const;
  double realLength() const;
  void draw(MYQCPPainter *painter, const MYQCPVector2D &pos, const MYQCPVector2D &dir) const;
  void draw(MYQCPPainter *painter, const MYQCPVector2D &pos, double angle) const;
  
protected:
  // property members:
  EndingStyle mStyle;
  double mWidth, mLength;
  bool mInverted;
};
Q_DECLARE_TYPEINFO(MYQCPLineEnding, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(MYQCPLineEnding::EndingStyle)

/* end of 'src/lineending.h' */


/* including file 'src/axis/axisticker.h', size 4224                         */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPAxisTicker
{
  Q_GADGET
public:
  /*!
    Defines the strategies that the axis ticker may follow when choosing the size of the tick step.
    
    \see setTickStepStrategy
  */
  enum TickStepStrategy
  {
    tssReadability    ///< A nicely readable tick step is prioritized over matching the requested number of ticks (see \ref setTickCount)
    ,tssMeetTickCount ///< Less readable tick steps are allowed which in turn facilitates getting closer to the requested tick count
  };
  Q_ENUMS(TickStepStrategy)
  
  MYQCPAxisTicker();
  virtual ~MYQCPAxisTicker();
  
  // getters:
  TickStepStrategy tickStepStrategy() const { return mTickStepStrategy; }
  int tickCount() const { return mTickCount; }
  double tickOrigin() const { return mTickOrigin; }
  
  // setters:
  void setTickStepStrategy(TickStepStrategy strategy);
  void setTickCount(int count);
  void setTickOrigin(double origin);
  
  // introduced virtual methods:
  virtual void generate(const MYQCPRange &range, const QLocale &locale, QChar formatChar, int precision, QVector<double> &ticks, QVector<double> *subTicks, QVector<QString> *tickLabels);
  
protected:
  // property members:
  TickStepStrategy mTickStepStrategy;
  int mTickCount;
  double mTickOrigin;
  
  // introduced virtual methods:
  virtual double getTickStep(const MYQCPRange &range);
  virtual int getSubTickCount(double tickStep);
  virtual QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision);
  virtual QVector<double> createTickVector(double tickStep, const MYQCPRange &range);
  virtual QVector<double> createSubTickVector(int subTickCount, const QVector<double> &ticks);
  virtual QVector<QString> createLabelVector(const QVector<double> &ticks, const QLocale &locale, QChar formatChar, int precision);
  
  // non-virtual methods:
  void trimTicks(const MYQCPRange &range, QVector<double> &ticks, bool keepOneOutlier) const;
  double pickClosest(double target, const QVector<double> &candidates) const;
  double getMantissa(double input, double *magnitude=0) const;
  double cleanMantissa(double input) const;
  
private:
  Q_DISABLE_COPY(MYQCPAxisTicker)
  
};
Q_DECLARE_METATYPE(MYQCPAxisTicker::TickStepStrategy)
Q_DECLARE_METATYPE(QSharedPointer<MYQCPAxisTicker>)

/* end of 'src/axis/axisticker.h' */


/* including file 'src/axis/axistickerdatetime.h', size 3289                 */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPAxisTickerDateTime : public MYQCPAxisTicker
{
public:
  MYQCPAxisTickerDateTime();
  
  // getters:
  QString dateTimeFormat() const { return mDateTimeFormat; }
  Qt::TimeSpec dateTimeSpec() const { return mDateTimeSpec; }
  
  // setters:
  void setDateTimeFormat(const QString &format);
  void setDateTimeSpec(Qt::TimeSpec spec);
  void setTickOrigin(double origin); // hides base class method but calls baseclass implementation ("using" throws off IDEs and doxygen)
  void setTickOrigin(const QDateTime &origin);
  
  // static methods:
  static QDateTime keyToDateTime(double key);
  static double dateTimeToKey(const QDateTime dateTime);
  static double dateTimeToKey(const QDate date);
  
protected:
  // property members:
  QString mDateTimeFormat;
  Qt::TimeSpec mDateTimeSpec;
  
  // non-property members:
  enum DateStrategy {dsNone, dsUniformTimeInDay, dsUniformDayInMonth} mDateStrategy;
  
  // reimplemented virtual methods:
  virtual double getTickStep(const MYQCPRange &range) Q_DECL_OVERRIDE;
  virtual int getSubTickCount(double tickStep) Q_DECL_OVERRIDE;
  virtual QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision) Q_DECL_OVERRIDE;
  virtual QVector<double> createTickVector(double tickStep, const MYQCPRange &range) Q_DECL_OVERRIDE;
};

/* end of 'src/axis/axistickerdatetime.h' */


/* including file 'src/axis/axistickertime.h', size 3542                     */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPAxisTickerTime : public MYQCPAxisTicker
{
  Q_GADGET
public:
  /*!
    Defines the logical units in which fractions of time spans can be expressed.
    
    \see setFieldWidth, setTimeFormat
  */
  enum TimeUnit { tuMilliseconds ///< Milliseconds, one thousandth of a second (%%z in \ref setTimeFormat)
                  ,tuSeconds     ///< Seconds (%%s in \ref setTimeFormat)
                  ,tuMinutes     ///< Minutes (%%m in \ref setTimeFormat)
                  ,tuHours       ///< Hours (%%h in \ref setTimeFormat)
                  ,tuDays        ///< Days (%%d in \ref setTimeFormat)
                };
  Q_ENUMS(TimeUnit)
  
  MYQCPAxisTickerTime();

  // getters:
  QString timeFormat() const { return mTimeFormat; }
  int fieldWidth(TimeUnit unit) const { return mFieldWidth.value(unit); }
  
  // setters:
  void setTimeFormat(const QString &format);
  void setFieldWidth(TimeUnit unit, int width);
  
protected:
  // property members:
  QString mTimeFormat;
  QHash<TimeUnit, int> mFieldWidth;
  
  // non-property members:
  TimeUnit mSmallestUnit, mBiggestUnit;
  QHash<TimeUnit, QString> mFormatPattern;
  
  // reimplemented virtual methods:
  virtual double getTickStep(const MYQCPRange &range) Q_DECL_OVERRIDE;
  virtual int getSubTickCount(double tickStep) Q_DECL_OVERRIDE;
  virtual QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision) Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  void replaceUnit(QString &text, TimeUnit unit, int value) const;
};
Q_DECLARE_METATYPE(MYQCPAxisTickerTime::TimeUnit)

/* end of 'src/axis/axistickertime.h' */


/* including file 'src/axis/axistickerfixed.h', size 3308                    */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPAxisTickerFixed : public MYQCPAxisTicker
{
  Q_GADGET
public:
  /*!
    Defines how the axis ticker may modify the specified tick step (\ref setTickStep) in order to
    control the number of ticks in the axis range.
    
    \see setScaleStrategy
  */
  enum ScaleStrategy { ssNone      ///< Modifications are not allowed, the specified tick step is absolutely fixed. This might cause a high tick density and overlapping labels if the axis range is zoomed out.
                       ,ssMultiples ///< An integer multiple of the specified tick step is allowed. The used factor follows the base class properties of \ref setTickStepStrategy and \ref setTickCount.
                       ,ssPowers    ///< An integer power of the specified tick step is allowed.
                     };
  Q_ENUMS(ScaleStrategy)
  
  MYQCPAxisTickerFixed();
  
  // getters:
  double tickStep() const { return mTickStep; }
  ScaleStrategy scaleStrategy() const { return mScaleStrategy; }
  
  // setters:
  void setTickStep(double step);
  void setScaleStrategy(ScaleStrategy strategy);
  
protected:
  // property members:
  double mTickStep;
  ScaleStrategy mScaleStrategy;
  
  // reimplemented virtual methods:
  virtual double getTickStep(const MYQCPRange &range) Q_DECL_OVERRIDE;
};
Q_DECLARE_METATYPE(MYQCPAxisTickerFixed::ScaleStrategy)

/* end of 'src/axis/axistickerfixed.h' */


/* including file 'src/axis/axistickertext.h', size 3090                     */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPAxisTickerText : public MYQCPAxisTicker
{
public:
  MYQCPAxisTickerText();
  
  // getters:
  QMap<double, QString> &ticks() { return mTicks; }
  int subTickCount() const { return mSubTickCount; }
  
  // setters:
  void setTicks(const QMap<double, QString> &ticks);
  void setTicks(const QVector<double> &positions, const QVector<QString> &labels);
  void setSubTickCount(int subTicks);
  
  // non-virtual methods:
  void clear();
  void addTick(double position, const QString &label);
  void addTicks(const QMap<double, QString> &ticks);
  void addTicks(const QVector<double> &positions, const QVector<QString> &labels);
  
protected:
  // property members:
  QMap<double, QString> mTicks;
  int mSubTickCount;
  
  // reimplemented virtual methods:
  virtual double getTickStep(const MYQCPRange &range) Q_DECL_OVERRIDE;
  virtual int getSubTickCount(double tickStep) Q_DECL_OVERRIDE;
  virtual QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision) Q_DECL_OVERRIDE;
  virtual QVector<double> createTickVector(double tickStep, const MYQCPRange &range) Q_DECL_OVERRIDE;
};

/* end of 'src/axis/axistickertext.h' */


/* including file 'src/axis/axistickerpi.h', size 3911                       */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPAxisTickerPi : public MYQCPAxisTicker
{
  Q_GADGET
public:
  /*!
    Defines how fractions should be displayed in tick labels.
    
    \see setFractionStyle
  */
  enum FractionStyle { fsFloatingPoint     ///< Fractions are displayed as regular decimal floating point numbers, e.g. "0.25" or "0.125".
                       ,fsAsciiFractions   ///< Fractions are written as rationals using ASCII characters only, e.g. "1/4" or "1/8"
                       ,fsUnicodeFractions ///< Fractions are written using sub- and superscript UTF-8 digits and the fraction symbol.
                     };
  Q_ENUMS(FractionStyle)
  
  MYQCPAxisTickerPi();
  
  // getters:
  QString piSymbol() const { return mPiSymbol; }
  double piValue() const { return mPiValue; }
  bool periodicity() const { return mPeriodicity; }
  FractionStyle fractionStyle() const { return mFractionStyle; }
  
  // setters:
  void setPiSymbol(QString symbol);
  void setPiValue(double pi);
  void setPeriodicity(int multiplesOfPi);
  void setFractionStyle(FractionStyle style);
  
protected:
  // property members:
  QString mPiSymbol;
  double mPiValue;
  int mPeriodicity;
  FractionStyle mFractionStyle;
  
  // non-property members:
  double mPiTickStep; // size of one tick step in units of mPiValue
  
  // reimplemented virtual methods:
  virtual double getTickStep(const MYQCPRange &range) Q_DECL_OVERRIDE;
  virtual int getSubTickCount(double tickStep) Q_DECL_OVERRIDE;
  virtual QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision) Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  void simplifyFraction(int &numerator, int &denominator) const;
  QString fractionToString(int numerator, int denominator) const;
  QString unicodeFraction(int numerator, int denominator) const;
  QString unicodeSuperscript(int number) const;
  QString unicodeSubscript(int number) const;
};
Q_DECLARE_METATYPE(MYQCPAxisTickerPi::FractionStyle)

/* end of 'src/axis/axistickerpi.h' */


/* including file 'src/axis/axistickerlog.h', size 2663                      */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPAxisTickerLog : public MYQCPAxisTicker
{
public:
  MYQCPAxisTickerLog();
  
  // getters:
  double logBase() const { return mLogBase; }
  int subTickCount() const { return mSubTickCount; }
  
  // setters:
  void setLogBase(double base);
  void setSubTickCount(int subTicks);
  
protected:
  // property members:
  double mLogBase;
  int mSubTickCount;
  
  // non-property members:
  double mLogBaseLnInv;
  
  // reimplemented virtual methods:
  virtual double getTickStep(const MYQCPRange &range) Q_DECL_OVERRIDE;
  virtual int getSubTickCount(double tickStep) Q_DECL_OVERRIDE;
  virtual QVector<double> createTickVector(double tickStep, const MYQCPRange &range) Q_DECL_OVERRIDE;
};

/* end of 'src/axis/axistickerlog.h' */


/* including file 'src/axis/axis.h', size 20698                              */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPGrid :public MYQCPLayerable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(bool subGridVisible READ subGridVisible WRITE setSubGridVisible)
  Q_PROPERTY(bool antialiasedSubGrid READ antialiasedSubGrid WRITE setAntialiasedSubGrid)
  Q_PROPERTY(bool antialiasedZeroLine READ antialiasedZeroLine WRITE setAntialiasedZeroLine)
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen subGridPen READ subGridPen WRITE setSubGridPen)
  Q_PROPERTY(QPen zeroLinePen READ zeroLinePen WRITE setZeroLinePen)
  /// \endcond
public:
  explicit MYQCPGrid(MYQCPAxis *parentAxis);
  
  // getters:
  bool subGridVisible() const { return mSubGridVisible; }
  bool antialiasedSubGrid() const { return mAntialiasedSubGrid; }
  bool antialiasedZeroLine() const { return mAntialiasedZeroLine; }
  QPen pen() const { return mPen; }
  QPen subGridPen() const { return mSubGridPen; }
  QPen zeroLinePen() const { return mZeroLinePen; }
  
  // setters:
  void setSubGridVisible(bool visible);
  void setAntialiasedSubGrid(bool enabled);
  void setAntialiasedZeroLine(bool enabled);
  void setPen(const QPen &pen);
  void setSubGridPen(const QPen &pen);
  void setZeroLinePen(const QPen &pen);
  
protected:
  // property members:
  bool mSubGridVisible;
  bool mAntialiasedSubGrid, mAntialiasedZeroLine;
  QPen mPen, mSubGridPen, mZeroLinePen;
  
  // non-property members:
  MYQCPAxis *mParentAxis;
  
  // reimplemented virtual methods:
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const Q_DECL_OVERRIDE;
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  void drawGridLines(MYQCPPainter *painter) const;
  void drawSubGridLines(MYQCPPainter *painter) const;
  
  friend class MYQCPAxis;
};


class MYQCP_LIB_DECL MYQCPAxis : public MYQCPLayerable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(AxisType axisType READ axisType)
  Q_PROPERTY(MYQCPAxisRect* axisRect READ axisRect)
  Q_PROPERTY(ScaleType scaleType READ scaleType WRITE setScaleType NOTIFY scaleTypeChanged)
  Q_PROPERTY(MYQCPRange range READ range WRITE setRange NOTIFY rangeChanged)
  Q_PROPERTY(bool rangeReversed READ rangeReversed WRITE setRangeReversed)
  Q_PROPERTY(QSharedPointer<MYQCPAxisTicker> ticker READ ticker WRITE setTicker)
  Q_PROPERTY(bool ticks READ ticks WRITE setTicks)
  Q_PROPERTY(bool tickLabels READ tickLabels WRITE setTickLabels)
  Q_PROPERTY(int tickLabelPadding READ tickLabelPadding WRITE setTickLabelPadding)
  Q_PROPERTY(QFont tickLabelFont READ tickLabelFont WRITE setTickLabelFont)
  Q_PROPERTY(QColor tickLabelColor READ tickLabelColor WRITE setTickLabelColor)
  Q_PROPERTY(double tickLabelRotation READ tickLabelRotation WRITE setTickLabelRotation)
  Q_PROPERTY(LabelSide tickLabelSide READ tickLabelSide WRITE setTickLabelSide)
  Q_PROPERTY(QString numberFormat READ numberFormat WRITE setNumberFormat)
  Q_PROPERTY(int numberPrecision READ numberPrecision WRITE setNumberPrecision)
  Q_PROPERTY(QVector<double> tickVector READ tickVector)
  Q_PROPERTY(QVector<QString> tickVectorLabels READ tickVectorLabels)
  Q_PROPERTY(int tickLengthIn READ tickLengthIn WRITE setTickLengthIn)
  Q_PROPERTY(int tickLengthOut READ tickLengthOut WRITE setTickLengthOut)
  Q_PROPERTY(bool subTicks READ subTicks WRITE setSubTicks)
  Q_PROPERTY(int subTickLengthIn READ subTickLengthIn WRITE setSubTickLengthIn)
  Q_PROPERTY(int subTickLengthOut READ subTickLengthOut WRITE setSubTickLengthOut)
  Q_PROPERTY(QPen basePen READ basePen WRITE setBasePen)
  Q_PROPERTY(QPen tickPen READ tickPen WRITE setTickPen)
  Q_PROPERTY(QPen subTickPen READ subTickPen WRITE setSubTickPen)
  Q_PROPERTY(QFont labelFont READ labelFont WRITE setLabelFont)
  Q_PROPERTY(QColor labelColor READ labelColor WRITE setLabelColor)
  Q_PROPERTY(QString label READ label WRITE setLabel)
  Q_PROPERTY(int labelPadding READ labelPadding WRITE setLabelPadding)
  Q_PROPERTY(int padding READ padding WRITE setPadding)
  Q_PROPERTY(int offset READ offset WRITE setOffset)
  Q_PROPERTY(SelectableParts selectedParts READ selectedParts WRITE setSelectedParts NOTIFY selectionChanged)
  Q_PROPERTY(SelectableParts selectableParts READ selectableParts WRITE setSelectableParts NOTIFY selectableChanged)
  Q_PROPERTY(QFont selectedTickLabelFont READ selectedTickLabelFont WRITE setSelectedTickLabelFont)
  Q_PROPERTY(QFont selectedLabelFont READ selectedLabelFont WRITE setSelectedLabelFont)
  Q_PROPERTY(QColor selectedTickLabelColor READ selectedTickLabelColor WRITE setSelectedTickLabelColor)
  Q_PROPERTY(QColor selectedLabelColor READ selectedLabelColor WRITE setSelectedLabelColor)
  Q_PROPERTY(QPen selectedBasePen READ selectedBasePen WRITE setSelectedBasePen)
  Q_PROPERTY(QPen selectedTickPen READ selectedTickPen WRITE setSelectedTickPen)
  Q_PROPERTY(QPen selectedSubTickPen READ selectedSubTickPen WRITE setSelectedSubTickPen)
  Q_PROPERTY(MYQCPLineEnding lowerEnding READ lowerEnding WRITE setLowerEnding)
  Q_PROPERTY(MYQCPLineEnding upperEnding READ upperEnding WRITE setUpperEnding)
  Q_PROPERTY(MYQCPGrid* grid READ grid)
  /// \endcond
public:
  /*!
    Defines at which side of the axis rect the axis will appear. This also affects how the tick
    marks are drawn, on which side the labels are placed etc.
  */
  enum AxisType { atLeft    = 0x01  ///< <tt>0x01</tt> Axis is vertical and on the left side of the axis rect
                  ,atRight  = 0x02  ///< <tt>0x02</tt> Axis is vertical and on the right side of the axis rect
                  ,atTop    = 0x04  ///< <tt>0x04</tt> Axis is horizontal and on the top side of the axis rect
                  ,atBottom = 0x08  ///< <tt>0x08</tt> Axis is horizontal and on the bottom side of the axis rect
                };
  Q_ENUMS(AxisType)
  Q_FLAGS(AxisTypes)
  Q_DECLARE_FLAGS(AxisTypes, AxisType)
  /*!
    Defines on which side of the axis the tick labels (numbers) shall appear.
    
    \see setTickLabelSide
  */
  enum LabelSide { lsInside    ///< Tick labels will be displayed inside the axis rect and clipped to the inner axis rect
                   ,lsOutside  ///< Tick labels will be displayed outside the axis rect
                 };
  Q_ENUMS(LabelSide)
  /*!
    Defines the scale of an axis.
    \see setScaleType
  */
  enum ScaleType { stLinear       ///< Linear scaling
                   ,stLogarithmic ///< Logarithmic scaling with correspondingly transformed axis coordinates (possibly also \ref setTicker to a \ref MYQCPAxisTickerLog instance).
                 };
  Q_ENUMS(ScaleType)
  /*!
    Defines the selectable parts of an axis.
    \see setSelectableParts, setSelectedParts
  */
  enum SelectablePart { spNone        = 0      ///< None of the selectable parts
                        ,spAxis       = 0x001  ///< The axis backbone and tick marks
                        ,spTickLabels = 0x002  ///< Tick labels (numbers) of this axis (as a whole, not individually)
                        ,spAxisLabel  = 0x004  ///< The axis label
                      };
  Q_ENUMS(SelectablePart)
  Q_FLAGS(SelectableParts)
  Q_DECLARE_FLAGS(SelectableParts, SelectablePart)
  
  explicit MYQCPAxis(MYQCPAxisRect *parent, AxisType type);
  virtual ~MYQCPAxis();
  
  // getters:
  AxisType axisType() const { return mAxisType; }
  MYQCPAxisRect *axisRect() const { return mAxisRect; }
  ScaleType scaleType() const { return mScaleType; }
  const MYQCPRange range() const { return mRange; }
  bool rangeReversed() const { return mRangeReversed; }
  QSharedPointer<MYQCPAxisTicker> ticker() const { return mTicker; }
  bool ticks() const { return mTicks; }
  bool tickLabels() const { return mTickLabels; }
  int tickLabelPadding() const;
  QFont tickLabelFont() const { return mTickLabelFont; }
  QColor tickLabelColor() const { return mTickLabelColor; }
  double tickLabelRotation() const;
  LabelSide tickLabelSide() const;
  QString numberFormat() const;
  int numberPrecision() const { return mNumberPrecision; }
  QVector<double> tickVector() const { return mTickVector; }
  QVector<QString> tickVectorLabels() const { return mTickVectorLabels; }
  int tickLengthIn() const;
  int tickLengthOut() const;
  bool subTicks() const { return mSubTicks; }
  int subTickLengthIn() const;
  int subTickLengthOut() const;
  QPen basePen() const { return mBasePen; }
  QPen tickPen() const { return mTickPen; }
  QPen subTickPen() const { return mSubTickPen; }
  QFont labelFont() const { return mLabelFont; }
  QColor labelColor() const { return mLabelColor; }
  QString label() const { return mLabel; }
  int labelPadding() const;
  int padding() const { return mPadding; }
  int offset() const;
  SelectableParts selectedParts() const { return mSelectedParts; }
  SelectableParts selectableParts() const { return mSelectableParts; }
  QFont selectedTickLabelFont() const { return mSelectedTickLabelFont; }
  QFont selectedLabelFont() const { return mSelectedLabelFont; }
  QColor selectedTickLabelColor() const { return mSelectedTickLabelColor; }
  QColor selectedLabelColor() const { return mSelectedLabelColor; }
  QPen selectedBasePen() const { return mSelectedBasePen; }
  QPen selectedTickPen() const { return mSelectedTickPen; }
  QPen selectedSubTickPen() const { return mSelectedSubTickPen; }
  MYQCPLineEnding lowerEnding() const;
  MYQCPLineEnding upperEnding() const;
  MYQCPGrid *grid() const { return mGrid; }
  
  // setters:
  Q_SLOT void setScaleType(MYQCPAxis::ScaleType type);
  Q_SLOT void setRange(const MYQCPRange &range);
  void setRange(double lower, double upper);
  void setRange(double position, double size, Qt::AlignmentFlag alignment);
  void setRangeLower(double lower);
  void setRangeUpper(double upper);
  void setRangeReversed(bool reversed);
  void setTicker(QSharedPointer<MYQCPAxisTicker> ticker);
  void setTicks(bool show);
  void setTickLabels(bool show);
  void setTickLabelPadding(int padding);
  void setTickLabelFont(const QFont &font);
  void setTickLabelColor(const QColor &color);
  void setTickLabelRotation(double degrees);
  void setTickLabelSide(LabelSide side);
  void setNumberFormat(const QString &formatCode);
  void setNumberPrecision(int precision);
  void setTickLength(int inside, int outside=0);
  void setTickLengthIn(int inside);
  void setTickLengthOut(int outside);
  void setSubTicks(bool show);
  void setSubTickLength(int inside, int outside=0);
  void setSubTickLengthIn(int inside);
  void setSubTickLengthOut(int outside);
  void setBasePen(const QPen &pen);
  void setTickPen(const QPen &pen);
  void setSubTickPen(const QPen &pen);
  void setLabelFont(const QFont &font);
  void setLabelColor(const QColor &color);
  void setLabel(const QString &str);
  void setLabelPadding(int padding);
  void setPadding(int padding);
  void setOffset(int offset);
  void setSelectedTickLabelFont(const QFont &font);
  void setSelectedLabelFont(const QFont &font);
  void setSelectedTickLabelColor(const QColor &color);
  void setSelectedLabelColor(const QColor &color);
  void setSelectedBasePen(const QPen &pen);
  void setSelectedTickPen(const QPen &pen);
  void setSelectedSubTickPen(const QPen &pen);
  Q_SLOT void setSelectableParts(const MYQCPAxis::SelectableParts &selectableParts);
  Q_SLOT void setSelectedParts(const MYQCPAxis::SelectableParts &selectedParts);
  void setLowerEnding(const MYQCPLineEnding &ending);
  void setUpperEnding(const MYQCPLineEnding &ending);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  
  // non-property methods:
  Qt::Orientation orientation() const { return mOrientation; }
  int pixelOrientation() const { return rangeReversed() != (orientation()==Qt::Vertical) ? -1 : 1; }
  void moveRange(double diff);
  void scaleRange(double factor);
  void scaleRange(double factor, double center);
  void setScaleRatio(const MYQCPAxis *otherAxis, double ratio=1.0);
  void rescale(bool onlyVisiblePlottables=false);
  double pixelToCoord(double value) const;
  double coordToPixel(double value) const;
  SelectablePart getPartAt(const QPointF &pos) const;
  QList<MYQCPAbstractPlottable*> plottables() const;
  QList<MYQCPGraph*> graphs() const;
  QList<MYQCPAbstractItem*> items() const;
  
  static AxisType marginSideToAxisType(MYQCP::MarginSide side);
  static Qt::Orientation orientation(AxisType type) { return type==atBottom||type==atTop ? Qt::Horizontal : Qt::Vertical; }
  static AxisType opposite(AxisType type);
  
signals:
  void rangeChanged(const MYQCPRange &newRange);
  void rangeChanged(const MYQCPRange &newRange, const MYQCPRange &oldRange);
  void scaleTypeChanged(MYQCPAxis::ScaleType scaleType);
  void selectionChanged(const MYQCPAxis::SelectableParts &parts);
  void selectableChanged(const MYQCPAxis::SelectableParts &parts);

protected:
  // property members:
  // axis base:
  AxisType mAxisType;
  MYQCPAxisRect *mAxisRect;
  //int mOffset; // in MYQCPAxisPainter
  int mPadding;
  Qt::Orientation mOrientation;
  SelectableParts mSelectableParts, mSelectedParts;
  QPen mBasePen, mSelectedBasePen;
  //MYQCPLineEnding mLowerEnding, mUpperEnding; // in MYQCPAxisPainter
  // axis label:
  //int mLabelPadding; // in MYQCPAxisPainter
  QString mLabel;
  QFont mLabelFont, mSelectedLabelFont;
  QColor mLabelColor, mSelectedLabelColor;
  // tick labels:
  //int mTickLabelPadding; // in MYQCPAxisPainter
  bool mTickLabels;
  //double mTickLabelRotation; // in MYQCPAxisPainter
  QFont mTickLabelFont, mSelectedTickLabelFont;
  QColor mTickLabelColor, mSelectedTickLabelColor;
  int mNumberPrecision;
  QLatin1Char mNumberFormatChar;
  bool mNumberBeautifulPowers;
  //bool mNumberMultiplyCross; // MYQCPAxisPainter
  // ticks and subticks:
  bool mTicks;
  bool mSubTicks;
  //int mTickLengthIn, mTickLengthOut, mSubTickLengthIn, mSubTickLengthOut; // MYQCPAxisPainter
  QPen mTickPen, mSelectedTickPen;
  QPen mSubTickPen, mSelectedSubTickPen;
  // scale and range:
  MYQCPRange mRange;
  bool mRangeReversed;
  ScaleType mScaleType;
  
  // non-property members:
  MYQCPGrid *mGrid;
  MYQCPAxisPainterPrivate *mAxisPainter;
  QSharedPointer<MYQCPAxisTicker> mTicker;
  QVector<double> mTickVector;
  QVector<QString> mTickVectorLabels;
  QVector<double> mSubTickVector;
  bool mCachedMarginValid;
  int mCachedMargin;
  bool mDragging;
  MYQCPRange mDragStartRange;
  MYQCP::AntialiasedElements mAADragBackup, mNotAADragBackup;
  
  // introduced virtual methods:
  virtual int calculateMargin();
  
  // reimplemented virtual methods:
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const Q_DECL_OVERRIDE;
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual MYQCP::Interaction selectionCategory() const Q_DECL_OVERRIDE;
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged) Q_DECL_OVERRIDE;
  virtual void deselectEvent(bool *selectionStateChanged) Q_DECL_OVERRIDE;
  // mouse events:
  virtual void mousePressEvent(QMouseEvent *event, const QVariant &details) Q_DECL_OVERRIDE;
  virtual void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos) Q_DECL_OVERRIDE;
  virtual void mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos) Q_DECL_OVERRIDE;
  virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  void setupTickVectors();
  QPen getBasePen() const;
  QPen getTickPen() const;
  QPen getSubTickPen() const;
  QFont getTickLabelFont() const;
  QFont getLabelFont() const;
  QColor getTickLabelColor() const;
  QColor getLabelColor() const;
  
private:
  Q_DISABLE_COPY(MYQCPAxis)
  
  friend class QCustomPlot;
  friend class MYQCPGrid;
  friend class MYQCPAxisRect;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(MYQCPAxis::SelectableParts)
Q_DECLARE_OPERATORS_FOR_FLAGS(MYQCPAxis::AxisTypes)
Q_DECLARE_METATYPE(MYQCPAxis::AxisType)
Q_DECLARE_METATYPE(MYQCPAxis::LabelSide)
Q_DECLARE_METATYPE(MYQCPAxis::ScaleType)
Q_DECLARE_METATYPE(MYQCPAxis::SelectablePart)


class MYQCPAxisPainterPrivate
{
public:
  explicit MYQCPAxisPainterPrivate(QCustomPlot *parentPlot);
  virtual ~MYQCPAxisPainterPrivate();
  
  virtual void draw(MYQCPPainter *painter);
  virtual int size() const;
  void clearCache();
  
  QRect axisSelectionBox() const { return mAxisSelectionBox; }
  QRect tickLabelsSelectionBox() const { return mTickLabelsSelectionBox; }
  QRect labelSelectionBox() const { return mLabelSelectionBox; }
  
  // public property members:
  MYQCPAxis::AxisType type;
  QPen basePen;
  MYQCPLineEnding lowerEnding, upperEnding; // directly accessed by MYQCPAxis setters/getters
  int labelPadding; // directly accessed by MYQCPAxis setters/getters
  QFont labelFont;
  QColor labelColor;
  QString label;
  int tickLabelPadding; // directly accessed by MYQCPAxis setters/getters
  double tickLabelRotation; // directly accessed by MYQCPAxis setters/getters
  MYQCPAxis::LabelSide tickLabelSide; // directly accessed by MYQCPAxis setters/getters
  bool substituteExponent;
  bool numberMultiplyCross; // directly accessed by MYQCPAxis setters/getters
  int tickLengthIn, tickLengthOut, subTickLengthIn, subTickLengthOut; // directly accessed by MYQCPAxis setters/getters
  QPen tickPen, subTickPen;
  QFont tickLabelFont;
  QColor tickLabelColor;
  QRect axisRect, viewportRect;
  double offset; // directly accessed by MYQCPAxis setters/getters
  bool abbreviateDecimalPowers;
  bool reversedEndings;
  
  QVector<double> subTickPositions;
  QVector<double> tickPositions;
  QVector<QString> tickLabels;
  
protected:
  struct CachedLabel
  {
    QPointF offset;
    QPixmap pixmap;
  };
  struct TickLabelData
  {
    QString basePart, expPart, suffixPart;
    QRect baseBounds, expBounds, suffixBounds, totalBounds, rotatedTotalBounds;
    QFont baseFont, expFont;
  };
  QCustomPlot *mParentPlot;
  QByteArray mLabelParameterHash; // to determine whether mLabelCache needs to be cleared due to changed parameters
  QCache<QString, CachedLabel> mLabelCache;
  QRect mAxisSelectionBox, mTickLabelsSelectionBox, mLabelSelectionBox;
  
  virtual QByteArray generateLabelParameterHash() const;
  
  virtual void placeTickLabel(MYQCPPainter *painter, double position, int distanceToAxis, const QString &text, QSize *tickLabelsSize);
  virtual void drawTickLabel(MYQCPPainter *painter, double x, double y, const TickLabelData &labelData) const;
  virtual TickLabelData getTickLabelData(const QFont &font, const QString &text) const;
  virtual QPointF getTickLabelDrawOffset(const TickLabelData &labelData) const;
  virtual void getMaxTickLabelSize(const QFont &font, const QString &text, QSize *tickLabelsSize) const;
};

/* end of 'src/axis/axis.h' */


/* including file 'src/scatterstyle.h', size 7275                            */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPScatterStyle
{
  Q_GADGET
public:
  /*!
    Represents the various properties of a scatter style instance. For example, this enum is used
    to specify which properties of \ref MYQCPSelectionDecorator::setScatterStyle will be used when
    highlighting selected data points.

    Specific scatter properties can be transferred between \ref MYQCPScatterStyle instances via \ref
    setFromOther.
  */
  enum ScatterProperty { spNone  = 0x00  ///< <tt>0x00</tt> None
                         ,spPen   = 0x01  ///< <tt>0x01</tt> The pen property, see \ref setPen
                         ,spBrush = 0x02  ///< <tt>0x02</tt> The brush property, see \ref setBrush
                         ,spSize  = 0x04  ///< <tt>0x04</tt> The size property, see \ref setSize
                         ,spShape = 0x08  ///< <tt>0x08</tt> The shape property, see \ref setShape
                         ,spAll   = 0xFF  ///< <tt>0xFF</tt> All properties
                       };
  Q_ENUMS(ScatterProperty)
  Q_FLAGS(ScatterProperties)
  Q_DECLARE_FLAGS(ScatterProperties, ScatterProperty)

  /*!
    Defines the shape used for scatter points.

    On plottables/items that draw scatters, the sizes of these visualizations (with exception of
    \ref ssDot and \ref ssPixmap) can be controlled with the \ref setSize function. Scatters are
    drawn with the pen and brush specified with \ref setPen and \ref setBrush.
  */
  enum ScatterShape { ssNone       ///< no scatter symbols are drawn (e.g. in MYQCPGraph, data only represented with lines)
                      ,ssDot       ///< \enumimage{ssDot.png} a single pixel (use \ref ssDisc or \ref ssCircle if you want a round shape with a certain radius)
                      ,ssCross     ///< \enumimage{ssCross.png} a cross
                      ,ssPlus      ///< \enumimage{ssPlus.png} a plus
                      ,ssCircle    ///< \enumimage{ssCircle.png} a circle
                      ,ssDisc      ///< \enumimage{ssDisc.png} a circle which is filled with the pen's color (not the brush as with ssCircle)
                      ,ssSquare    ///< \enumimage{ssSquare.png} a square
                      ,ssDiamond   ///< \enumimage{ssDiamond.png} a diamond
                      ,ssStar      ///< \enumimage{ssStar.png} a star with eight arms, i.e. a combination of cross and plus
                      ,ssTriangle  ///< \enumimage{ssTriangle.png} an equilateral triangle, standing on baseline
                      ,ssTriangleInverted ///< \enumimage{ssTriangleInverted.png} an equilateral triangle, standing on corner
                      ,ssCrossSquare      ///< \enumimage{ssCrossSquare.png} a square with a cross inside
                      ,ssPlusSquare       ///< \enumimage{ssPlusSquare.png} a square with a plus inside
                      ,ssCrossCircle      ///< \enumimage{ssCrossCircle.png} a circle with a cross inside
                      ,ssPlusCircle       ///< \enumimage{ssPlusCircle.png} a circle with a plus inside
                      ,ssPeace     ///< \enumimage{ssPeace.png} a circle, with one vertical and two downward diagonal lines
                      ,ssPixmap    ///< a custom pixmap specified by \ref setPixmap, centered on the data point coordinates
                      ,ssCustom    ///< custom painter operations are performed per scatter (As QPainterPath, see \ref setCustomPath)
                    };
  Q_ENUMS(ScatterShape)

  MYQCPScatterStyle();
  MYQCPScatterStyle(ScatterShape shape, double size=6);
  MYQCPScatterStyle(ScatterShape shape, const QColor &color, double size);
  MYQCPScatterStyle(ScatterShape shape, const QColor &color, const QColor &fill, double size);
  MYQCPScatterStyle(ScatterShape shape, const QPen &pen, const QBrush &brush, double size);
  MYQCPScatterStyle(const QPixmap &pixmap);
  MYQCPScatterStyle(const QPainterPath &customPath, const QPen &pen, const QBrush &brush=Qt::NoBrush, double size=6);
  
  // getters:
  double size() const { return mSize; }
  ScatterShape shape() const { return mShape; }
  QPen pen() const { return mPen; }
  QBrush brush() const { return mBrush; }
  QPixmap pixmap() const { return mPixmap; }
  QPainterPath customPath() const { return mCustomPath; }

  // setters:
  void setFromOther(const MYQCPScatterStyle &other, ScatterProperties properties);
  void setSize(double size);
  void setShape(ScatterShape shape);
  void setPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setPixmap(const QPixmap &pixmap);
  void setCustomPath(const QPainterPath &customPath);

  // non-property methods:
  bool isNone() const { return mShape == ssNone; }
  bool isPenDefined() const { return mPenDefined; }
  void undefinePen();
  void applyTo(MYQCPPainter *painter, const QPen &defaultPen) const;
  void drawShape(MYQCPPainter *painter, const QPointF &pos) const;
  void drawShape(MYQCPPainter *painter, double x, double y) const;

protected:
  // property members:
  double mSize;
  ScatterShape mShape;
  QPen mPen;
  QBrush mBrush;
  QPixmap mPixmap;
  QPainterPath mCustomPath;
  
  // non-property members:
  bool mPenDefined;
};
Q_DECLARE_TYPEINFO(MYQCPScatterStyle, Q_MOVABLE_TYPE);
Q_DECLARE_OPERATORS_FOR_FLAGS(MYQCPScatterStyle::ScatterProperties)
Q_DECLARE_METATYPE(MYQCPScatterStyle::ScatterProperty)
Q_DECLARE_METATYPE(MYQCPScatterStyle::ScatterShape)

/* end of 'src/scatterstyle.h' */


/* including file 'src/datacontainer.h', size 4596                           */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

/*! \relates MYQCPDataContainer
  Returns whether the sort key of \a a is less than the sort key of \a b.

  \see MYQCPDataContainer::sort
*/
template <class DataType>
inline bool MYQCPLessThanSortKey(const DataType &a, const DataType &b) { return a.sortKey() < b.sortKey(); }

template <class DataType>
class MYQCPDataContainer // no MYQCP_LIB_DECL, template class ends up in header (cpp included below)
{
public:
  typedef typename QVector<DataType>::const_iterator const_iterator;
  typedef typename QVector<DataType>::iterator iterator;
  
  MYQCPDataContainer();
  
  // getters:
  int size() const { return mData.size()-mPreallocSize; }
  bool isEmpty() const { return size() == 0; }
  bool autoSqueeze() const { return mAutoSqueeze; }
  
  // setters:
  void setAutoSqueeze(bool enabled);
  
  // non-virtual methods:
  void set(const MYQCPDataContainer<DataType> &data);
  void set(const QVector<DataType> &data, bool alreadySorted=false);
  void add(const MYQCPDataContainer<DataType> &data);
  void add(const QVector<DataType> &data, bool alreadySorted=false);
  void add(const DataType &data);
  void removeBefore(double sortKey);
  void removeAfter(double sortKey);
  void remove(double sortKeyFrom, double sortKeyTo);
  void remove(double sortKey);
  void clear();
  void sort();
  void squeeze(bool preAllocation=true, bool postAllocation=true);
  
  const_iterator constBegin() const { return mData.constBegin()+mPreallocSize; }
  const_iterator constEnd() const { return mData.constEnd(); }
  iterator begin() { return mData.begin()+mPreallocSize; }
  iterator end() { return mData.end(); }
  const_iterator findBegin(double sortKey, bool expandedRange=true) const;
  const_iterator findEnd(double sortKey, bool expandedRange=true) const;
  const_iterator at(int index) const { return constBegin()+qBound(0, index, size()); }
  MYQCPRange keyRange(bool &foundRange, MYQCP::SignDomain signDomain=MYQCP::sdBoth);
  MYQCPRange valueRange(bool &foundRange, MYQCP::SignDomain signDomain=MYQCP::sdBoth, const MYQCPRange &inKeyRange=MYQCPRange());
  MYQCPDataRange dataRange() const { return MYQCPDataRange(0, size()); }
  void limitIteratorsToDataRange(const_iterator &begin, const_iterator &end, const MYQCPDataRange &dataRange) const;
  
protected:
  // property members:
  bool mAutoSqueeze;
  
  // non-property memebers:
  QVector<DataType> mData;
  int mPreallocSize;
  int mPreallocIteration;
  
  // non-virtual methods:
  void preallocateGrow(int minimumPreallocSize);
  void performAutoSqueeze();
};

// include implementation in header since it is a class template:

/* including file 'src/datacontainer.cpp', size 31349                        */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// MYQCPDataContainer
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class MYQCPDataContainer
  \brief The generic data container for one-dimensional plottables

  This class template provides a fast container for data storage of one-dimensional data. The data
  type is specified as template parameter (called \a DataType in the following) and must provide
  some methods as described in the \ref MYQCPdatacontainer-datatype "next section".

  The data is stored in a sorted fashion, which allows very quick lookups by the sorted key as well
  as retrieval of ranges (see \ref findBegin, \ref findEnd, \ref keyRange) using binary search. The
  container uses a preallocation and a postallocation scheme, such that appending and prepending
  data (with respect to the sort key) is very fast and minimizes reallocations. If data is added
  which needs to be inserted between existing keys, the merge usually can be done quickly too,
  using the fact that existing data is always sorted. The user can further improve performance by
  specifying that added data is already itself sorted by key, if he can guarantee that this is the
  case (see for example \ref add(const QVector<DataType> &data, bool alreadySorted)).

  The data can be accessed with the provided const iterators (\ref constBegin, \ref constEnd). If
  it is necessary to alter existing data in-place, the non-const iterators can be used (\ref begin,
  \ref end). Changing data members that are not the sort key (for most data types called \a key) is
  safe from the container's perspective.

  Great care must be taken however if the sort key is modified through the non-const iterators. For
  performance reasons, the iterators don't automatically cause a re-sorting upon their
  manipulation. It is thus the responsibility of the user to leave the container in a sorted state
  when finished with the data manipulation, before calling any other methods on the container. A
  complete re-sort (e.g. after finishing all sort key manipulation) can be done by calling \ref
  sort. Failing to do so can not be detected by the container efficiently and will cause both
  rendering artifacts and potential data loss.

  Implementing one-dimensional plottables that make use of a \ref MYQCPDataContainer<T> is usually
  done by subclassing from \ref MYQCPAbstractPlottable1D "MYQCPAbstractPlottable1D<T>", which
  introduces an according \a mDataContainer member and some convenience methods.

  \section MYQCPdatacontainer-datatype Requirements for the DataType template parameter

  The template parameter <tt>DataType</tt> is the type of the stored data points. It must be
  trivially copyable and have the following public methods, preferably inline:

  \li <tt>double sortKey() const</tt>\n Returns the member variable of this data point that is the
  sort key, defining the ordering in the container. Often this variable is simply called \a key.

  \li <tt>static DataType fromSortKey(double sortKey)</tt>\n Returns a new instance of the data
  type initialized with its sort key set to \a sortKey.

  \li <tt>static bool sortKeyIsMainKey()</tt>\n Returns true if the sort key is equal to the main
  key (see method \c mainKey below). For most plottables this is the case. It is not the case for
  example for \ref MYQCPCurve, which uses \a t as sort key and \a key as main key. This is the reason
  why MYQCPCurve unlike MYQCPGraph can display parametric curves with loops.

  \li <tt>double mainKey() const</tt>\n Returns the variable of this data point considered the main
  key. This is commonly the variable that is used as the coordinate of this data point on the key
  axis of the plottable. This method is used for example when determining the automatic axis
  rescaling of key axes (\ref MYQCPAxis::rescale).

  \li <tt>double mainValue() const</tt>\n Returns the variable of this data point considered the
  main value. This is commonly the variable that is used as the coordinate of this data point on
  the value axis of the plottable.

  \li <tt>MYQCPRange valueRange() const</tt>\n Returns the range this data point spans in the value
  axis coordinate. If the data is single-valued (e.g. MYQCPGraphData), this is simply a range with
  both lower and upper set to the main data point value. However if the data points can represent
  multiple values at once (e.g MYQCPFinancialData with its \a high, \a low, \a open and \a close
  values at each \a key) this method should return the range those values span. This method is used
  for example when determining the automatic axis rescaling of value axes (\ref
  MYQCPAxis::rescale).
*/

/* start documentation of inline functions */

/*! \fn int MYQCPDataContainer<DataType>::size() const
  
  Returns the number of data points in the container.
*/

/*! \fn bool MYQCPDataContainer<DataType>::isEmpty() const
  
  Returns whether this container holds no data points.
*/

/*! \fn MYQCPDataContainer::const_iterator MYQCPDataContainer<DataType>::constBegin() const
  
  Returns a const iterator to the first data point in this container.
*/

/*! \fn MYQCPDataContainer::const_iterator MYQCPDataContainer<DataType>::constEnd() const
  
  Returns a const iterator to the element past the last data point in this container.
*/

/*! \fn MYQCPDataContainer::iterator MYQCPDataContainer<DataType>::begin() const
  
  Returns a non-const iterator to the first data point in this container.

  You can manipulate the data points in-place through the non-const iterators, but great care must
  be taken when manipulating the sort key of a data point, see \ref sort, or the detailed
  description of this class.
*/

/*! \fn MYQCPDataContainer::iterator MYQCPDataContainer<DataType>::end() const
  
  Returns a non-const iterator to the element past the last data point in this container.
  
  You can manipulate the data points in-place through the non-const iterators, but great care must
  be taken when manipulating the sort key of a data point, see \ref sort, or the detailed
  description of this class.
*/

/*! \fn MYQCPDataContainer::const_iterator MYQCPDataContainer<DataType>::at(int index) const

  Returns a const iterator to the element with the specified \a index. If \a index points beyond
  the available elements in this container, returns \ref constEnd, i.e. an iterator past the last
  valid element.

  You can use this method to easily obtain iterators from a \ref MYQCPDataRange, see the \ref
  dataselection-accessing "data selection page" for an example.
*/

/*! \fn MYQCPDataRange MYQCPDataContainer::dataRange() const

  Returns a \ref MYQCPDataRange encompassing the entire data set of this container. This means the
  begin index of the returned range is 0, and the end index is \ref size.
*/

/* end documentation of inline functions */

/*!
  Constructs a MYQCPDataContainer used for plottable classes that represent a series of key-sorted
  data
*/
template <class DataType>
MYQCPDataContainer<DataType>::MYQCPDataContainer() :
  mAutoSqueeze(true),
  mPreallocSize(0),
  mPreallocIteration(0)
{
}

/*!
  Sets whether the container automatically decides when to release memory from its post- and
  preallocation pools when data points are removed. By default this is enabled and for typical
  applications shouldn't be changed.
  
  If auto squeeze is disabled, you can manually decide when to release pre-/postallocation with
  \ref squeeze.
*/
template <class DataType>
void MYQCPDataContainer<DataType>::setAutoSqueeze(bool enabled)
{
  if (mAutoSqueeze != enabled)
  {
    mAutoSqueeze = enabled;
    if (mAutoSqueeze)
      performAutoSqueeze();
  }
}

/*! \overload
  
  Replaces the current data in this container with the provided \a data.
  
  \see add, remove
*/
template <class DataType>
void MYQCPDataContainer<DataType>::set(const MYQCPDataContainer<DataType> &data)
{
  clear();
  add(data);
}

/*! \overload
  
  Replaces the current data in this container with the provided \a data

  If you can guarantee that the data points in \a data have ascending order with respect to the
  DataType's sort key, set \a alreadySorted to true to avoid an unnecessary sorting run.
  
  \see add, remove
*/
template <class DataType>
void MYQCPDataContainer<DataType>::set(const QVector<DataType> &data, bool alreadySorted)
{
  mData = data;
  mPreallocSize = 0;
  mPreallocIteration = 0;
  if (!alreadySorted)
    sort();
}

/*! \overload
  
  Adds the provided \a data to the current data in this container.
  
  \see set, remove
*/
template <class DataType>
void MYQCPDataContainer<DataType>::add(const MYQCPDataContainer<DataType> &data)
{
  if (data.isEmpty())
    return;
  
  const int n = data.size();
  const int oldSize = size();
  
  if (oldSize > 0 && !MYQCPLessThanSortKey<DataType>(*constBegin(), *(data.constEnd()-1))) // prepend if new data keys are all smaller than or equal to existing ones
  {
    if (mPreallocSize < n)
      preallocateGrow(n);
    mPreallocSize -= n;
    std::copy(data.constBegin(), data.constEnd(), begin());
  } else // don't need to prepend, so append and merge if necessary
  {
    mData.resize(mData.size()+n);
    std::copy(data.constBegin(), data.constEnd(), end()-n);
    if (oldSize > 0 && !MYQCPLessThanSortKey<DataType>(*(constEnd()-n-1), *(constEnd()-n))) // if appended range keys aren't all greater than existing ones, merge the two partitions
      std::inplace_merge(begin(), end()-n, end(), MYQCPLessThanSortKey<DataType>);
  }
}

/*!
  Adds the provided data points in \a data to the current data.
  
  If you can guarantee that the data points in \a data have ascending order with respect to the
  DataType's sort key, set \a alreadySorted to true to avoid an unnecessary sorting run.
  
  \see set, remove
*/
template <class DataType>
void MYQCPDataContainer<DataType>::add(const QVector<DataType> &data, bool alreadySorted)
{
  if (data.isEmpty())
    return;
  if (isEmpty())
  {
    set(data, alreadySorted);
    return;
  }
  
  const int n = data.size();
  const int oldSize = size();
  
  if (alreadySorted && oldSize > 0 && !MYQCPLessThanSortKey<DataType>(*constBegin(), *(data.constEnd()-1))) // prepend if new data is sorted and keys are all smaller than or equal to existing ones
  {
    if (mPreallocSize < n)
      preallocateGrow(n);
    mPreallocSize -= n;
    std::copy(data.constBegin(), data.constEnd(), begin());
  } else // don't need to prepend, so append and then sort and merge if necessary
  {
    mData.resize(mData.size()+n);
    std::copy(data.constBegin(), data.constEnd(), end()-n);
    if (!alreadySorted) // sort appended subrange if it wasn't already sorted
      std::sort(end()-n, end(), MYQCPLessThanSortKey<DataType>);
    if (oldSize > 0 && !MYQCPLessThanSortKey<DataType>(*(constEnd()-n-1), *(constEnd()-n))) // if appended range keys aren't all greater than existing ones, merge the two partitions
      std::inplace_merge(begin(), end()-n, end(), MYQCPLessThanSortKey<DataType>);
  }
}

/*! \overload
  
  Adds the provided single data point to the current data.
  
  \see remove
*/
template <class DataType>
void MYQCPDataContainer<DataType>::add(const DataType &data)
{
  if (isEmpty() || !MYQCPLessThanSortKey<DataType>(data, *(constEnd()-1))) // quickly handle appends if new data key is greater or equal to existing ones
  {
    mData.append(data);
  } else if (MYQCPLessThanSortKey<DataType>(data, *constBegin()))  // quickly handle prepends using preallocated space
  {
    if (mPreallocSize < 1)
      preallocateGrow(1);
    --mPreallocSize;
    *begin() = data;
  } else // handle inserts, maintaining sorted keys
  {
    MYQCPDataContainer<DataType>::iterator insertionPoint = std::lower_bound(begin(), end(), data, MYQCPLessThanSortKey<DataType>);
    mData.insert(insertionPoint, data);
  }
}

/*!
  Removes all data points with (sort-)keys smaller than or equal to \a sortKey.
  
  \see removeAfter, remove, clear
*/
template <class DataType>
void MYQCPDataContainer<DataType>::removeBefore(double sortKey)
{
  MYQCPDataContainer<DataType>::iterator it = begin();
  MYQCPDataContainer<DataType>::iterator itEnd = std::lower_bound(begin(), end(), DataType::fromSortKey(sortKey), MYQCPLessThanSortKey<DataType>);
  mPreallocSize += itEnd-it; // don't actually delete, just add it to the preallocated block (if it gets too large, squeeze will take care of it)
  if (mAutoSqueeze)
    performAutoSqueeze();
}

/*!
  Removes all data points with (sort-)keys greater than or equal to \a sortKey.

  \see removeBefore, remove, clear
*/
template <class DataType>
void MYQCPDataContainer<DataType>::removeAfter(double sortKey)
{
  MYQCPDataContainer<DataType>::iterator it = std::upper_bound(begin(), end(), DataType::fromSortKey(sortKey), MYQCPLessThanSortKey<DataType>);
  MYQCPDataContainer<DataType>::iterator itEnd = end();
  mData.erase(it, itEnd); // typically adds it to the postallocated block
  if (mAutoSqueeze)
    performAutoSqueeze();
}

/*!
  Removes all data points with (sort-)keys between \a sortKeyFrom and \a sortKeyTo. if \a
  sortKeyFrom is greater or equal to \a sortKeyTo, the function does nothing. To remove a single
  data point with known (sort-)key, use \ref remove(double sortKey).
  
  \see removeBefore, removeAfter, clear
*/
template <class DataType>
void MYQCPDataContainer<DataType>::remove(double sortKeyFrom, double sortKeyTo)
{
  if (sortKeyFrom >= sortKeyTo || isEmpty())
    return;
  
  MYQCPDataContainer<DataType>::iterator it = std::lower_bound(begin(), end(), DataType::fromSortKey(sortKeyFrom), MYQCPLessThanSortKey<DataType>);
  MYQCPDataContainer<DataType>::iterator itEnd = std::upper_bound(it, end(), DataType::fromSortKey(sortKeyTo), MYQCPLessThanSortKey<DataType>);
  mData.erase(it, itEnd);
  if (mAutoSqueeze)
    performAutoSqueeze();
}

/*! \overload
  
  Removes a single data point at \a sortKey. If the position is not known with absolute (binary)
  precision, consider using \ref remove(double sortKeyFrom, double sortKeyTo) with a small
  fuzziness interval around the suspected position, depeding on the precision with which the
  (sort-)key is known.
  
  \see removeBefore, removeAfter, clear
*/
template <class DataType>
void MYQCPDataContainer<DataType>::remove(double sortKey)
{
  MYQCPDataContainer::iterator it = std::lower_bound(begin(), end(), DataType::fromSortKey(sortKey), MYQCPLessThanSortKey<DataType>);
  if (it != end() && it->sortKey() == sortKey)
  {
    if (it == begin())
      ++mPreallocSize; // don't actually delete, just add it to the preallocated block (if it gets too large, squeeze will take care of it)
    else
      mData.erase(it);
  }
  if (mAutoSqueeze)
    performAutoSqueeze();
}

/*!
  Removes all data points.
  
  \see remove, removeAfter, removeBefore
*/
template <class DataType>
void MYQCPDataContainer<DataType>::clear()
{
  mData.clear();
  mPreallocIteration = 0;
  mPreallocSize = 0;
}

/*!
  Re-sorts all data points in the container by their sort key.

  When setting, adding or removing points using the MYQCPDataContainer interface (\ref set, \ref add,
  \ref remove, etc.), the container makes sure to always stay in a sorted state such that a full
  resort is never necessary. However, if you choose to directly manipulate the sort key on data
  points by accessing and modifying it through the non-const iterators (\ref begin, \ref end), it
  is your responsibility to bring the container back into a sorted state before any other methods
  are called on it. This can be achieved by calling this method immediately after finishing the
  sort key manipulation.
*/
template <class DataType>
void MYQCPDataContainer<DataType>::sort()
{
  std::sort(begin(), end(), MYQCPLessThanSortKey<DataType>);
}

/*!
  Frees all unused memory that is currently in the preallocation and postallocation pools.
  
  Note that MYQCPDataContainer automatically decides whether squeezing is necessary, if \ref
  setAutoSqueeze is left enabled. It should thus not be necessary to use this method for typical
  applications.
  
  The parameters \a preAllocation and \a postAllocation control whether pre- and/or post allocation
  should be freed, respectively.
*/
template <class DataType>
void MYQCPDataContainer<DataType>::squeeze(bool preAllocation, bool postAllocation)
{
  if (preAllocation)
  {
    if (mPreallocSize > 0)
    {
      std::copy(begin(), end(), mData.begin());
      mData.resize(size());
      mPreallocSize = 0;
    }
    mPreallocIteration = 0;
  }
  if (postAllocation)
    mData.squeeze();
}

/*!
  Returns an iterator to the data point with a (sort-)key that is equal to, just below, or just
  above \a sortKey. If \a expandedRange is true, the data point just below \a sortKey will be
  considered, otherwise the one just above.

  This can be used in conjunction with \ref findEnd to iterate over data points within a given key
  range, including or excluding the bounding data points that are just beyond the specified range.

  If \a expandedRange is true but there are no data points below \a sortKey, \ref constBegin is
  returned.

  If the container is empty, returns \ref constEnd.

  \see findEnd, MYQCPPlottableInterface1D::findBegin
*/
template <class DataType>
typename MYQCPDataContainer<DataType>::const_iterator MYQCPDataContainer<DataType>::findBegin(double sortKey, bool expandedRange) const
{
  if (isEmpty())
    return constEnd();
  
  MYQCPDataContainer<DataType>::const_iterator it = std::lower_bound(constBegin(), constEnd(), DataType::fromSortKey(sortKey), MYQCPLessThanSortKey<DataType>);
  if (expandedRange && it != constBegin()) // also covers it == constEnd case, and we know --constEnd is valid because mData isn't empty
    --it;
  return it;
}

/*!
  Returns an iterator to the element after the data point with a (sort-)key that is equal to, just
  above or just below \a sortKey. If \a expandedRange is true, the data point just above \a sortKey
  will be considered, otherwise the one just below.

  This can be used in conjunction with \ref findBegin to iterate over data points within a given
  key range, including the bounding data points that are just below and above the specified range.

  If \a expandedRange is true but there are no data points above \a sortKey, \ref constEnd is
  returned.

  If the container is empty, \ref constEnd is returned.

  \see findBegin, MYQCPPlottableInterface1D::findEnd
*/
template <class DataType>
typename MYQCPDataContainer<DataType>::const_iterator MYQCPDataContainer<DataType>::findEnd(double sortKey, bool expandedRange) const
{
  if (isEmpty())
    return constEnd();
  
  MYQCPDataContainer<DataType>::const_iterator it = std::upper_bound(constBegin(), constEnd(), DataType::fromSortKey(sortKey), MYQCPLessThanSortKey<DataType>);
  if (expandedRange && it != constEnd())
    ++it;
  return it;
}

/*!
  Returns the range encompassed by the (main-)key coordinate of all data points. The output
  parameter \a foundRange indicates whether a sensible range was found. If this is false, you
  should not use the returned MYQCPRange (e.g. the data container is empty or all points have the
  same key).
  
  Use \a signDomain to control which sign of the key coordinates should be considered. This is
  relevant e.g. for logarithmic plots which can mathematically only display one sign domain at a
  time.
  
  If the DataType reports that its main key is equal to the sort key (\a sortKeyIsMainKey), as is
  the case for most plottables, this method uses this fact and finds the range very quickly.
  
  \see valueRange
*/
template <class DataType>
MYQCPRange MYQCPDataContainer<DataType>::keyRange(bool &foundRange, MYQCP::SignDomain signDomain)
{
  if (isEmpty())
  {
    foundRange = false;
    return MYQCPRange();
  }
  MYQCPRange range;
  bool haveLower = false;
  bool haveUpper = false;
  double current;
  
  MYQCPDataContainer<DataType>::const_iterator it = constBegin();
  MYQCPDataContainer<DataType>::const_iterator itEnd = constEnd();
  if (signDomain == MYQCP::sdBoth) // range may be anywhere
  {
    if (DataType::sortKeyIsMainKey()) // if DataType is sorted by main key (e.g. MYQCPGraph, but not MYQCPCurve), use faster algorithm by finding just first and last key with non-NaN value
    {
      while (it != itEnd) // find first non-nan going up from left
      {
        if (!qIsNaN(it->mainValue()))
        {
          range.lower = it->mainKey();
          haveLower = true;
          break;
        }
        ++it;
      }
      it = itEnd;
      while (it != constBegin()) // find first non-nan going down from right
      {
        --it;
        if (!qIsNaN(it->mainValue()))
        {
          range.upper = it->mainKey();
          haveUpper = true;
          break;
        }
      }
    } else // DataType is not sorted by main key, go through all data points and accordingly expand range
    {
      while (it != itEnd)
      {
        if (!qIsNaN(it->mainValue()))
        {
          current = it->mainKey();
          if (current < range.lower || !haveLower)
          {
            range.lower = current;
            haveLower = true;
          }
          if (current > range.upper || !haveUpper)
          {
            range.upper = current;
            haveUpper = true;
          }
        }
        ++it;
      }
    }
  } else if (signDomain == MYQCP::sdNegative) // range may only be in the negative sign domain
  {
    while (it != itEnd)
    {
      if (!qIsNaN(it->mainValue()))
      {
        current = it->mainKey();
        if ((current < range.lower || !haveLower) && current < 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current < 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  } else if (signDomain == MYQCP::sdPositive) // range may only be in the positive sign domain
  {
    while (it != itEnd)
    {
      if (!qIsNaN(it->mainValue()))
      {
        current = it->mainKey();
        if ((current < range.lower || !haveLower) && current > 0)
        {
          range.lower = current;
          haveLower = true;
        }
        if ((current > range.upper || !haveUpper) && current > 0)
        {
          range.upper = current;
          haveUpper = true;
        }
      }
      ++it;
    }
  }
  
  foundRange = haveLower && haveUpper;
  return range;
}

/*!
  Returns the range encompassed by the value coordinates of the data points in the specified key
  range (\a inKeyRange), using the full \a DataType::valueRange reported by the data points. The
  output parameter \a foundRange indicates whether a sensible range was found. If this is false,
  you should not use the returned MYQCPRange (e.g. the data container is empty or all points have the
  same value).

  If \a inKeyRange has both lower and upper bound set to zero (is equal to <tt>MYQCPRange()</tt>),
  all data points are considered, without any restriction on the keys.

  Use \a signDomain to control which sign of the value coordinates should be considered. This is
  relevant e.g. for logarithmic plots which can mathematically only display one sign domain at a
  time.

  \see keyRange
*/
template <class DataType>
MYQCPRange MYQCPDataContainer<DataType>::valueRange(bool &foundRange, MYQCP::SignDomain signDomain, const MYQCPRange &inKeyRange)
{
  if (isEmpty())
  {
    foundRange = false;
    return MYQCPRange();
  }
  MYQCPRange range;
  const bool restrictKeyRange = inKeyRange != MYQCPRange();
  bool haveLower = false;
  bool haveUpper = false;
  MYQCPRange current;
  MYQCPDataContainer<DataType>::const_iterator itBegin = constBegin();
  MYQCPDataContainer<DataType>::const_iterator itEnd = constEnd();
  if (DataType::sortKeyIsMainKey() && restrictKeyRange)
  {
    itBegin = findBegin(inKeyRange.lower);
    itEnd = findEnd(inKeyRange.upper);
  }
  if (signDomain == MYQCP::sdBoth) // range may be anywhere
  {
    for (MYQCPDataContainer<DataType>::const_iterator it = itBegin; it != itEnd; ++it)
    {
      if (restrictKeyRange && (it->mainKey() < inKeyRange.lower || it->mainKey() > inKeyRange.upper))
        continue;
      current = it->valueRange();
      if ((current.lower < range.lower || !haveLower) && !qIsNaN(current.lower))
      {
        range.lower = current.lower;
        haveLower = true;
      }
      if ((current.upper > range.upper || !haveUpper) && !qIsNaN(current.upper))
      {
        range.upper = current.upper;
        haveUpper = true;
      }
    }
  } else if (signDomain == MYQCP::sdNegative) // range may only be in the negative sign domain
  {
    for (MYQCPDataContainer<DataType>::const_iterator it = itBegin; it != itEnd; ++it)
    {
      if (restrictKeyRange && (it->mainKey() < inKeyRange.lower || it->mainKey() > inKeyRange.upper))
        continue;
      current = it->valueRange();
      if ((current.lower < range.lower || !haveLower) && current.lower < 0 && !qIsNaN(current.lower))
      {
        range.lower = current.lower;
        haveLower = true;
      }
      if ((current.upper > range.upper || !haveUpper) && current.upper < 0 && !qIsNaN(current.upper))
      {
        range.upper = current.upper;
        haveUpper = true;
      }
    }
  } else if (signDomain == MYQCP::sdPositive) // range may only be in the positive sign domain
  {
    for (MYQCPDataContainer<DataType>::const_iterator it = itBegin; it != itEnd; ++it)
    {
      if (restrictKeyRange && (it->mainKey() < inKeyRange.lower || it->mainKey() > inKeyRange.upper))
        continue;
      current = it->valueRange();
      if ((current.lower < range.lower || !haveLower) && current.lower > 0 && !qIsNaN(current.lower))
      {
        range.lower = current.lower;
        haveLower = true;
      }
      if ((current.upper > range.upper || !haveUpper) && current.upper > 0 && !qIsNaN(current.upper))
      {
        range.upper = current.upper;
        haveUpper = true;
      }
    }
  }
  
  foundRange = haveLower && haveUpper;
  return range;
}

/*!
  Makes sure \a begin and \a end mark a data range that is both within the bounds of this data
  container's data, as well as within the specified \a dataRange. The initial range described by
  the passed iterators \a begin and \a end is never expanded, only contracted if necessary.
  
  This function doesn't require for \a dataRange to be within the bounds of this data container's
  valid range.
*/
template <class DataType>
void MYQCPDataContainer<DataType>::limitIteratorsToDataRange(const_iterator &begin, const_iterator &end, const MYQCPDataRange &dataRange) const
{
  MYQCPDataRange iteratorRange(begin-constBegin(), end-constBegin());
  iteratorRange = iteratorRange.bounded(dataRange.bounded(this->dataRange()));
  begin = constBegin()+iteratorRange.begin();
  end = constBegin()+iteratorRange.end();
}

/*! \internal
  
  Increases the preallocation pool to have a size of at least \a minimumPreallocSize. Depending on
  the preallocation history, the container will grow by more than requested, to speed up future
  consecutive size increases.
  
  if \a minimumPreallocSize is smaller than or equal to the current preallocation pool size, this
  method does nothing.
*/
template <class DataType>
void MYQCPDataContainer<DataType>::preallocateGrow(int minimumPreallocSize)
{
  if (minimumPreallocSize <= mPreallocSize)
    return;
  
  int newPreallocSize = minimumPreallocSize;
  newPreallocSize += (1u<<qBound(4, mPreallocIteration+4, 15)) - 12; // do 4 up to 32768-12 preallocation, doubling in each intermediate iteration
  ++mPreallocIteration;
  
  int sizeDifference = newPreallocSize-mPreallocSize;
  mData.resize(mData.size()+sizeDifference);
  std::copy_backward(mData.begin()+mPreallocSize, mData.end()-sizeDifference, mData.end());
  mPreallocSize = newPreallocSize;
}

/*! \internal
  
  This method decides, depending on the total allocation size and the size of the unused pre- and
  postallocation pools, whether it is sensible to reduce the pools in order to free up unused
  memory. It then possibly calls \ref squeeze to do the deallocation.
  
  If \ref setAutoSqueeze is enabled, this method is called automatically each time data points are
  removed from the container (e.g. \ref remove).
  
  \note when changing the decision parameters, care must be taken not to cause a back-and-forth
  between squeezing and reallocation due to the growth strategy of the internal QVector and \ref
  preallocateGrow. The hysteresis between allocation and deallocation should be made high enough
  (at the expense of possibly larger unused memory from time to time).
*/
template <class DataType>
void MYQCPDataContainer<DataType>::performAutoSqueeze()
{
  const int totalAlloc = mData.capacity();
  const int postAllocSize = totalAlloc-mData.size();
  const int usedSize = size();
  bool shrinkPostAllocation = false;
  bool shrinkPreAllocation = false;
  if (totalAlloc > 650000) // if allocation is larger, shrink earlier with respect to total used size
  {
    shrinkPostAllocation = postAllocSize > usedSize*1.5; // QVector grow strategy is 2^n for static data. Watch out not to oscillate!
    shrinkPreAllocation = mPreallocSize*10 > usedSize;
  } else if (totalAlloc > 1000) // below 10 MiB raw data be generous with preallocated memory, below 1k points don't even bother
  {
    shrinkPostAllocation = postAllocSize > usedSize*5;
    shrinkPreAllocation = mPreallocSize > usedSize*1.5; // preallocation can grow into postallocation, so can be smaller
  }
  
  if (shrinkPreAllocation || shrinkPostAllocation)
    squeeze(shrinkPreAllocation, shrinkPostAllocation);
}
/* end of 'src/datacontainer.cpp' */


/* end of 'src/datacontainer.h' */


/* including file 'src/plottable.h', size 8433                               */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPSelectionDecorator
{
  Q_GADGET
public:
  MYQCPSelectionDecorator();
  virtual ~MYQCPSelectionDecorator();
  
  // getters:
  QPen pen() const { return mPen; }
  QBrush brush() const { return mBrush; }
  MYQCPScatterStyle scatterStyle() const { return mScatterStyle; }
  MYQCPScatterStyle::ScatterProperties usedScatterProperties() const { return mUsedScatterProperties; }
  
  // setters:
  void setPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setScatterStyle(const MYQCPScatterStyle &scatterStyle, MYQCPScatterStyle::ScatterProperties usedProperties=MYQCPScatterStyle::spPen);
  void setUsedScatterProperties(const MYQCPScatterStyle::ScatterProperties &properties);
  
  // non-virtual methods:
  void applyPen(MYQCPPainter *painter) const;
  void applyBrush(MYQCPPainter *painter) const;
  MYQCPScatterStyle getFinalScatterStyle(const MYQCPScatterStyle &unselectedStyle) const;
  
  // introduced virtual methods:
  virtual void copyFrom(const MYQCPSelectionDecorator *other);
  virtual void drawDecoration(MYQCPPainter *painter, MYQCPDataSelection selection);
  
protected:
  // property members:
  QPen mPen;
  QBrush mBrush;
  MYQCPScatterStyle mScatterStyle;
  MYQCPScatterStyle::ScatterProperties mUsedScatterProperties;
  // non-property members:
  MYQCPAbstractPlottable *mPlottable;
  
  // introduced virtual methods:
  virtual bool registerWithPlottable(MYQCPAbstractPlottable *plottable);
  
private:
  Q_DISABLE_COPY(MYQCPSelectionDecorator)
  friend class MYQCPAbstractPlottable;
};
Q_DECLARE_METATYPE(MYQCPSelectionDecorator*)


class MYQCP_LIB_DECL MYQCPAbstractPlottable : public MYQCPLayerable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QString name READ name WRITE setName)
  Q_PROPERTY(bool antialiasedFill READ antialiasedFill WRITE setAntialiasedFill)
  Q_PROPERTY(bool antialiasedScatters READ antialiasedScatters WRITE setAntialiasedScatters)
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
  Q_PROPERTY(MYQCPAxis* keyAxis READ keyAxis WRITE setKeyAxis)
  Q_PROPERTY(MYQCPAxis* valueAxis READ valueAxis WRITE setValueAxis)
  Q_PROPERTY(MYQCP::SelectionType selectable READ selectable WRITE setSelectable NOTIFY selectableChanged)
  Q_PROPERTY(MYQCPDataSelection selection READ selection WRITE setSelection NOTIFY selectionChanged)
  Q_PROPERTY(MYQCPSelectionDecorator* selectionDecorator READ selectionDecorator WRITE setSelectionDecorator)
  /// \endcond
public:
  MYQCPAbstractPlottable(MYQCPAxis *keyAxis, MYQCPAxis *valueAxis);
  virtual ~MYQCPAbstractPlottable();
  
  // getters:
  QString name() const { return mName; }
  bool antialiasedFill() const { return mAntialiasedFill; }
  bool antialiasedScatters() const { return mAntialiasedScatters; }
  QPen pen() const { return mPen; }
  QBrush brush() const { return mBrush; }
  MYQCPAxis *keyAxis() const { return mKeyAxis.data(); }
  MYQCPAxis *valueAxis() const { return mValueAxis.data(); }
  MYQCP::SelectionType selectable() const { return mSelectable; }
  bool selected() const { return !mSelection.isEmpty(); }
  MYQCPDataSelection selection() const { return mSelection; }
  MYQCPSelectionDecorator *selectionDecorator() const { return mSelectionDecorator; }
  
  // setters:
  void setName(const QString &name);
  void setAntialiasedFill(bool enabled);
  void setAntialiasedScatters(bool enabled);
  void setPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setKeyAxis(MYQCPAxis *axis);
  void setValueAxis(MYQCPAxis *axis);
  Q_SLOT void setSelectable(MYQCP::SelectionType selectable);
  Q_SLOT void setSelection(MYQCPDataSelection selection);
  void setSelectionDecorator(MYQCPSelectionDecorator *decorator);

  // introduced virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE = 0; // actually introduced in MYQCPLayerable as non-pure, but we want to force reimplementation for plottables
  virtual MYQCPPlottableInterface1D *interface1D() { return 0; }
  virtual MYQCPRange getKeyRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth) const = 0;
  virtual MYQCPRange getValueRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth, const MYQCPRange &inKeyRange=MYQCPRange()) const = 0;
  
  // non-property methods:
  void coordsToPixels(double key, double value, double &x, double &y) const;
  const QPointF coordsToPixels(double key, double value) const;
  void pixelsToCoords(double x, double y, double &key, double &value) const;
  void pixelsToCoords(const QPointF &pixelPos, double &key, double &value) const;
  void rescaleAxes(bool onlyEnlarge=false) const;
  void rescaleKeyAxis(bool onlyEnlarge=false) const;
  void rescaleValueAxis(bool onlyEnlarge=false, bool inKeyRange=false) const;
  bool addToLegend(MYQCPLegend *legend);
  bool addToLegend();
  bool removeFromLegend(MYQCPLegend *legend) const;
  bool removeFromLegend() const;
  
signals:
  void selectionChanged(bool selected);
  void selectionChanged(const MYQCPDataSelection &selection);
  void selectableChanged(MYQCP::SelectionType selectable);
  
protected:
  // property members:
  QString mName;
  bool mAntialiasedFill, mAntialiasedScatters;
  QPen mPen;
  QBrush mBrush;
  QPointer<MYQCPAxis> mKeyAxis, mValueAxis;
  MYQCP::SelectionType mSelectable;
  MYQCPDataSelection mSelection;
  MYQCPSelectionDecorator *mSelectionDecorator;
  
  // reimplemented virtual methods:
  virtual QRect clipRect() const Q_DECL_OVERRIDE;
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE = 0;
  virtual MYQCP::Interaction selectionCategory() const Q_DECL_OVERRIDE;
  void applyDefaultAntialiasingHint(MYQCPPainter *painter) const Q_DECL_OVERRIDE;
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged) Q_DECL_OVERRIDE;
  virtual void deselectEvent(bool *selectionStateChanged) Q_DECL_OVERRIDE;
  
  // introduced virtual methods:
  virtual void drawLegendIcon(MYQCPPainter *painter, const QRectF &rect) const = 0;
  
  // non-virtual methods:
  void applyFillAntialiasingHint(MYQCPPainter *painter) const;
  void applyScattersAntialiasingHint(MYQCPPainter *painter) const;

private:
  Q_DISABLE_COPY(MYQCPAbstractPlottable)
  
  friend class QCustomPlot;
  friend class MYQCPAxis;
  friend class MYQCPPlottableLegendItem;
};


/* end of 'src/plottable.h' */


/* including file 'src/item.h', size 9384                                    */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPItemAnchor
{
  Q_GADGET
public:
  MYQCPItemAnchor(QCustomPlot *parentPlot, MYQCPAbstractItem *parentItem, const QString &name, int anchorId=-1);
  virtual ~MYQCPItemAnchor();
  
  // getters:
  QString name() const { return mName; }
  virtual QPointF pixelPosition() const;
  
protected:
  // property members:
  QString mName;
  
  // non-property members:
  QCustomPlot *mParentPlot;
  MYQCPAbstractItem *mParentItem;
  int mAnchorId;
  QSet<MYQCPItemPosition*> mChildrenX, mChildrenY;
  
  // introduced virtual methods:
  virtual MYQCPItemPosition *toMYQCPItemPosition() { return 0; }
  
  // non-virtual methods:
  void addChildX(MYQCPItemPosition* pos); // called from pos when this anchor is set as parent
  void removeChildX(MYQCPItemPosition *pos); // called from pos when its parent anchor is reset or pos deleted
  void addChildY(MYQCPItemPosition* pos); // called from pos when this anchor is set as parent
  void removeChildY(MYQCPItemPosition *pos); // called from pos when its parent anchor is reset or pos deleted
  
private:
  Q_DISABLE_COPY(MYQCPItemAnchor)
  
  friend class MYQCPItemPosition;
};



class MYQCP_LIB_DECL MYQCPItemPosition : public MYQCPItemAnchor
{
  Q_GADGET
public:
  /*!
    Defines the ways an item position can be specified. Thus it defines what the numbers passed to
    \ref setCoords actually mean.
    
    \see setType
  */
  enum PositionType { ptAbsolute        ///< Static positioning in pixels, starting from the top left corner of the viewport/widget.
                      ,ptViewportRatio  ///< Static positioning given by a fraction of the viewport size. For example, if you call setCoords(0, 0), the position will be at the top
                                        ///< left corner of the viewport/widget. setCoords(1, 1) will be at the bottom right corner, setCoords(0.5, 0) will be horizontally centered and
                                        ///< vertically at the top of the viewport/widget, etc.
                      ,ptAxisRectRatio  ///< Static positioning given by a fraction of the axis rect size (see \ref setAxisRect). For example, if you call setCoords(0, 0), the position will be at the top
                                        ///< left corner of the axis rect. setCoords(1, 1) will be at the bottom right corner, setCoords(0.5, 0) will be horizontally centered and
                                        ///< vertically at the top of the axis rect, etc. You can also go beyond the axis rect by providing negative coordinates or coordinates larger than 1.
                      ,ptPlotCoords     ///< Dynamic positioning at a plot coordinate defined by two axes (see \ref setAxes).
                    };
  Q_ENUMS(PositionType)
  
  MYQCPItemPosition(QCustomPlot *parentPlot, MYQCPAbstractItem *parentItem, const QString &name);
  virtual ~MYQCPItemPosition();
  
  // getters:
  PositionType type() const { return typeX(); }
  PositionType typeX() const { return mPositionTypeX; }
  PositionType typeY() const { return mPositionTypeY; }
  MYQCPItemAnchor *parentAnchor() const { return parentAnchorX(); }
  MYQCPItemAnchor *parentAnchorX() const { return mParentAnchorX; }
  MYQCPItemAnchor *parentAnchorY() const { return mParentAnchorY; }
  double key() const { return mKey; }
  double value() const { return mValue; }
  QPointF coords() const { return QPointF(mKey, mValue); }
  MYQCPAxis *keyAxis() const { return mKeyAxis.data(); }
  MYQCPAxis *valueAxis() const { return mValueAxis.data(); }
  MYQCPAxisRect *axisRect() const;
  virtual QPointF pixelPosition() const Q_DECL_OVERRIDE;
  
  // setters:
  void setType(PositionType type);
  void setTypeX(PositionType type);
  void setTypeY(PositionType type);
  bool setParentAnchor(MYQCPItemAnchor *parentAnchor, bool keepPixelPosition=false);
  bool setParentAnchorX(MYQCPItemAnchor *parentAnchor, bool keepPixelPosition=false);
  bool setParentAnchorY(MYQCPItemAnchor *parentAnchor, bool keepPixelPosition=false);
  void setCoords(double key, double value);
  void setCoords(const QPointF &coords);
  void setAxes(MYQCPAxis* keyAxis, MYQCPAxis* valueAxis);
  void setAxisRect(MYQCPAxisRect *axisRect);
  void setPixelPosition(const QPointF &pixelPosition);
  
protected:
  // property members:
  PositionType mPositionTypeX, mPositionTypeY;
  QPointer<MYQCPAxis> mKeyAxis, mValueAxis;
  QPointer<MYQCPAxisRect> mAxisRect;
  double mKey, mValue;
  MYQCPItemAnchor *mParentAnchorX, *mParentAnchorY;
  
  // reimplemented virtual methods:
  virtual MYQCPItemPosition *toMYQCPItemPosition() Q_DECL_OVERRIDE { return this; }
  
private:
  Q_DISABLE_COPY(MYQCPItemPosition)
  
};
Q_DECLARE_METATYPE(MYQCPItemPosition::PositionType)


class MYQCP_LIB_DECL MYQCPAbstractItem : public MYQCPLayerable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(bool clipToAxisRect READ clipToAxisRect WRITE setClipToAxisRect)
  Q_PROPERTY(MYQCPAxisRect* clipAxisRect READ clipAxisRect WRITE setClipAxisRect)
  Q_PROPERTY(bool selectable READ selectable WRITE setSelectable NOTIFY selectableChanged)
  Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectionChanged)
  /// \endcond
public:
  explicit MYQCPAbstractItem(QCustomPlot *parentPlot);
  virtual ~MYQCPAbstractItem();
  
  // getters:
  bool clipToAxisRect() const { return mClipToAxisRect; }
  MYQCPAxisRect *clipAxisRect() const;
  bool selectable() const { return mSelectable; }
  bool selected() const { return mSelected; }
  
  // setters:
  void setClipToAxisRect(bool clip);
  void setClipAxisRect(MYQCPAxisRect *rect);
  Q_SLOT void setSelectable(bool selectable);
  Q_SLOT void setSelected(bool selected);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE = 0;
  
  // non-virtual methods:
  QList<MYQCPItemPosition*> positions() const { return mPositions; }
  QList<MYQCPItemAnchor*> anchors() const { return mAnchors; }
  MYQCPItemPosition *position(const QString &name) const;
  MYQCPItemAnchor *anchor(const QString &name) const;
  bool hasAnchor(const QString &name) const;
  
signals:
  void selectionChanged(bool selected);
  void selectableChanged(bool selectable);
  
protected:
  // property members:
  bool mClipToAxisRect;
  QPointer<MYQCPAxisRect> mClipAxisRect;
  QList<MYQCPItemPosition*> mPositions;
  QList<MYQCPItemAnchor*> mAnchors;
  bool mSelectable, mSelected;
  
  // reimplemented virtual methods:
  virtual MYQCP::Interaction selectionCategory() const Q_DECL_OVERRIDE;
  virtual QRect clipRect() const Q_DECL_OVERRIDE;
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const Q_DECL_OVERRIDE;
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE = 0;
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged) Q_DECL_OVERRIDE;
  virtual void deselectEvent(bool *selectionStateChanged) Q_DECL_OVERRIDE;
  
  // introduced virtual methods:
  virtual QPointF anchorPixelPosition(int anchorId) const;
  
  // non-virtual methods:
  double rectDistance(const QRectF &rect, const QPointF &pos, bool filledRect) const;
  MYQCPItemPosition *createPosition(const QString &name);
  MYQCPItemAnchor *createAnchor(const QString &name, int anchorId);
  
private:
  Q_DISABLE_COPY(MYQCPAbstractItem)
  
  friend class QCustomPlot;
  friend class MYQCPItemAnchor;
};

/* end of 'src/item.h' */


/* including file 'src/core.h', size 14886                                   */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL QCustomPlot : public QWidget
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QRect viewport READ viewport WRITE setViewport)
  Q_PROPERTY(QPixmap background READ background WRITE setBackground)
  Q_PROPERTY(bool backgroundScaled READ backgroundScaled WRITE setBackgroundScaled)
  Q_PROPERTY(Qt::AspectRatioMode backgroundScaledMode READ backgroundScaledMode WRITE setBackgroundScaledMode)
  Q_PROPERTY(MYQCPLayoutGrid* plotLayout READ plotLayout)
  Q_PROPERTY(bool autoAddPlottableToLegend READ autoAddPlottableToLegend WRITE setAutoAddPlottableToLegend)
  Q_PROPERTY(int selectionTolerance READ selectionTolerance WRITE setSelectionTolerance)
  Q_PROPERTY(bool noAntialiasingOnDrag READ noAntialiasingOnDrag WRITE setNoAntialiasingOnDrag)
  Q_PROPERTY(Qt::KeyboardModifier multiSelectModifier READ multiSelectModifier WRITE setMultiSelectModifier)
  Q_PROPERTY(bool openGl READ openGl WRITE setOpenGl)
  /// \endcond
public:
  /*!
    Defines how a layer should be inserted relative to an other layer.

    \see addLayer, moveLayer
  */
  enum LayerInsertMode { limBelow  ///< Layer is inserted below other layer
                         ,limAbove ///< Layer is inserted above other layer
                       };
  Q_ENUMS(LayerInsertMode)
  
  /*!
    Defines with what timing the QCustomPlot surface is refreshed after a replot.

    \see replot
  */
  enum RefreshPriority { rpImmediateRefresh ///< Replots immediately and repaints the widget immediately by calling QWidget::repaint() after the replot
                         ,rpQueuedRefresh   ///< Replots immediately, but queues the widget repaint, by calling QWidget::update() after the replot. This way multiple redundant widget repaints can be avoided.
                         ,rpRefreshHint     ///< Whether to use immediate or queued refresh depends on whether the plotting hint \ref MYQCP::phImmediateRefresh is set, see \ref setPlottingHints.
                         ,rpQueuedReplot    ///< Queues the entire replot for the next event loop iteration. This way multiple redundant replots can be avoided. The actual replot is then done with \ref rpRefreshHint priority.
                       };
  Q_ENUMS(RefreshPriority)
  
  explicit QCustomPlot(QWidget *parent = 0);
  virtual ~QCustomPlot();
  
  // getters:
  QRect viewport() const { return mViewport; }
  double bufferDevicePixelRatio() const { return mBufferDevicePixelRatio; }
  QPixmap background() const { return mBackgroundPixmap; }
  bool backgroundScaled() const { return mBackgroundScaled; }
  Qt::AspectRatioMode backgroundScaledMode() const { return mBackgroundScaledMode; }
  MYQCPLayoutGrid *plotLayout() const { return mPlotLayout; }
  MYQCP::AntialiasedElements antialiasedElements() const { return mAntialiasedElements; }
  MYQCP::AntialiasedElements notAntialiasedElements() const { return mNotAntialiasedElements; }
  bool autoAddPlottableToLegend() const { return mAutoAddPlottableToLegend; }
  const MYQCP::Interactions interactions() const { return mInteractions; }
  int selectionTolerance() const { return mSelectionTolerance; }
  bool noAntialiasingOnDrag() const { return mNoAntialiasingOnDrag; }
  MYQCP::PlottingHints plottingHints() const { return mPlottingHints; }
  Qt::KeyboardModifier multiSelectModifier() const { return mMultiSelectModifier; }
  MYQCP::SelectionRectMode selectionRectMode() const { return mSelectionRectMode; }
  MYQCPSelectionRect *selectionRect() const { return mSelectionRect; }
  bool openGl() const { return mOpenGl; }
  
  // setters:
  void setViewport(const QRect &rect);
  void setBufferDevicePixelRatio(double ratio);
  void setBackground(const QPixmap &pm);
  void setBackground(const QPixmap &pm, bool scaled, Qt::AspectRatioMode mode=Qt::KeepAspectRatioByExpanding);
  void setBackground(const QBrush &brush);
  void setBackgroundScaled(bool scaled);
  void setBackgroundScaledMode(Qt::AspectRatioMode mode);
  void setAntialiasedElements(const MYQCP::AntialiasedElements &antialiasedElements);
  void setAntialiasedElement(MYQCP::AntialiasedElement antialiasedElement, bool enabled=true);
  void setNotAntialiasedElements(const MYQCP::AntialiasedElements &notAntialiasedElements);
  void setNotAntialiasedElement(MYQCP::AntialiasedElement notAntialiasedElement, bool enabled=true);
  void setAutoAddPlottableToLegend(bool on);
  void setInteractions(const MYQCP::Interactions &interactions);
  void setInteraction(const MYQCP::Interaction &interaction, bool enabled=true);
  void setSelectionTolerance(int pixels);
  void setNoAntialiasingOnDrag(bool enabled);
  void setPlottingHints(const MYQCP::PlottingHints &hints);
  void setPlottingHint(MYQCP::PlottingHint hint, bool enabled=true);
  void setMultiSelectModifier(Qt::KeyboardModifier modifier);
  void setSelectionRectMode(MYQCP::SelectionRectMode mode);
  void setSelectionRect(MYQCPSelectionRect *selectionRect);
  void setOpenGl(bool enabled, int multisampling=16);
  

  virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
  virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
  virtual void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

  // non-property methods:
  // plottable interface:
  MYQCPAbstractPlottable *plottable(int index);
  MYQCPAbstractPlottable *plottable();
  bool removePlottable(MYQCPAbstractPlottable *plottable);
  bool removePlottable(int index);
  int clearPlottables();
  int plottableCount() const;
  QList<MYQCPAbstractPlottable*> selectedPlottables() const;
  MYQCPAbstractPlottable *plottableAt(const QPointF &pos, bool onlySelectable=false) const;
  bool hasPlottable(MYQCPAbstractPlottable *plottable) const;
 
  // specialized interface for MYQCPGraph:
  MYQCPGraph *graph(int index) const;
  MYQCPGraph *graph() const;
  MYQCPGraph *addGraph(MYQCPAxis *keyAxis=0, MYQCPAxis *valueAxis=0);
  bool removeGraph(MYQCPGraph *graph);
  bool removeGraph(int index);
  int clearGraphs();
  int graphCount() const;
  QList<MYQCPGraph*> selectedGraphs() const;

  // item interface:
  MYQCPAbstractItem *item(int index) const;
  MYQCPAbstractItem *item() const;
  bool removeItem(MYQCPAbstractItem *item);
  bool removeItem(int index);
  int clearItems();
  int itemCount() const;
  QList<MYQCPAbstractItem*> selectedItems() const;
  MYQCPAbstractItem *itemAt(const QPointF &pos, bool onlySelectable=false) const;
  bool hasItem(MYQCPAbstractItem *item) const;
  
  // layer interface:
  MYQCPLayer *layer(const QString &name) const;
  MYQCPLayer *layer(int index) const;
  MYQCPLayer *currentLayer() const;
  bool setCurrentLayer(const QString &name);
  bool setCurrentLayer(MYQCPLayer *layer);
  int layerCount() const;
  bool addLayer(const QString &name, MYQCPLayer *otherLayer=0, LayerInsertMode insertMode=limAbove);
  bool removeLayer(MYQCPLayer *layer);
  bool moveLayer(MYQCPLayer *layer, MYQCPLayer *otherLayer, LayerInsertMode insertMode=limAbove);
  
  // axis rect/layout interface:
  int axisRectCount() const;
  MYQCPAxisRect* axisRect(int index=0) const;
  QList<MYQCPAxisRect*> axisRects() const;
  MYQCPLayoutElement* layoutElementAt(const QPointF &pos) const;
  MYQCPAxisRect* axisRectAt(const QPointF &pos) const;
  Q_SLOT void rescaleAxes(bool onlyVisiblePlottables=false);
  
  QList<MYQCPAxis*> selectedAxes() const;
  QList<MYQCPLegend*> selectedLegends() const;
  Q_SLOT void deselectAll();
  
  bool savePdf(const QString &fileName, int width=0, int height=0, MYQCP::ExportPen exportPen=MYQCP::epAllowCosmetic, const QString &pdfCreator=QString(), const QString &pdfTitle=QString());
  bool savePng(const QString &fileName, int width=0, int height=0, double scale=1.0, int quality=-1, int resolution=96, MYQCP::ResolutionUnit resolutionUnit=MYQCP::ruDotsPerInch);
  bool saveJpg(const QString &fileName, int width=0, int height=0, double scale=1.0, int quality=-1, int resolution=96, MYQCP::ResolutionUnit resolutionUnit=MYQCP::ruDotsPerInch);
  bool saveBmp(const QString &fileName, int width=0, int height=0, double scale=1.0, int resolution=96, MYQCP::ResolutionUnit resolutionUnit=MYQCP::ruDotsPerInch);
  bool saveRastered(const QString &fileName, int width, int height, double scale, const char *format, int quality=-1, int resolution=96, MYQCP::ResolutionUnit resolutionUnit=MYQCP::ruDotsPerInch);
  QPixmap toPixmap(int width=0, int height=0, double scale=1.0);
  void toPainter(MYQCPPainter *painter, int width=0, int height=0);
  Q_SLOT void replot(QCustomPlot::RefreshPriority refreshPriority=QCustomPlot::rpRefreshHint);
  
  MYQCPAxis *xAxis, *yAxis, *xAxis2, *yAxis2;
  MYQCPLegend *legend;
  
signals:
  void mouseDoubleClick(QMouseEvent *event);
  void mousePress(QMouseEvent *event);
  void mouseMove(QMouseEvent *event);
  void mouseRelease(QMouseEvent *event);
  void mouseWheel(QWheelEvent *event);
  
  void plottableClick(MYQCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
  void plottableDoubleClick(MYQCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
  void itemClick(MYQCPAbstractItem *item, QMouseEvent *event);
  void itemDoubleClick(MYQCPAbstractItem *item, QMouseEvent *event);
  void axisClick(MYQCPAxis *axis, MYQCPAxis::SelectablePart part, QMouseEvent *event);
  void axisDoubleClick(MYQCPAxis *axis, MYQCPAxis::SelectablePart part, QMouseEvent *event);
  void legendClick(MYQCPLegend *legend, MYQCPAbstractLegendItem *item, QMouseEvent *event);
  void legendDoubleClick(MYQCPLegend *legend,  MYQCPAbstractLegendItem *item, QMouseEvent *event);
  
  void selectionChangedByUser();
  void beforeReplot();
  void afterReplot();
  
protected:
  // property members:
  QRect mViewport;
  double mBufferDevicePixelRatio;
  MYQCPLayoutGrid *mPlotLayout;
  bool mAutoAddPlottableToLegend;
  QList<MYQCPAbstractPlottable*> mPlottables;
  QList<MYQCPGraph*> mGraphs; // extra list of plottables also in mPlottables that are of type MYQCPGraph
  QList<MYQCPAbstractItem*> mItems;
  QList<MYQCPLayer*> mLayers;
  MYQCP::AntialiasedElements mAntialiasedElements, mNotAntialiasedElements;
  MYQCP::Interactions mInteractions;
  int mSelectionTolerance;
  bool mNoAntialiasingOnDrag;
  QBrush mBackgroundBrush;
  QPixmap mBackgroundPixmap;
  QPixmap mScaledBackgroundPixmap;
  bool mBackgroundScaled;
  Qt::AspectRatioMode mBackgroundScaledMode;
  MYQCPLayer *mCurrentLayer;
  MYQCP::PlottingHints mPlottingHints;
  Qt::KeyboardModifier mMultiSelectModifier;
  MYQCP::SelectionRectMode mSelectionRectMode;
  MYQCPSelectionRect *mSelectionRect;
  bool mOpenGl;
  
  // non-property members:
  QList<QSharedPointer<MYQCPAbstractPaintBuffer> > mPaintBuffers;
  QPoint mMousePressPos;
  bool mMouseHasMoved;
  QPointer<MYQCPLayerable> mMouseEventLayerable;
  QPointer<MYQCPLayerable> mMouseSignalLayerable;
  QVariant mMouseEventLayerableDetails;
  QVariant mMouseSignalLayerableDetails;
  bool mReplotting;
  bool mReplotQueued;
  int mOpenGlMultisamples;
  MYQCP::AntialiasedElements mOpenGlAntialiasedElementsBackup;
  bool mOpenGlCacheLabelsBackup;
#ifdef MYQCP_OPENGL_FBO
  QSharedPointer<QOpenGLContext> mGlContext;
  QSharedPointer<QSurface> mGlSurface;
  QSharedPointer<QOpenGLPaintDevice> mGlPaintDevice;
#endif
  
  // reimplemented virtual methods:
  virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;
  virtual QSize sizeHint() const Q_DECL_OVERRIDE;
  virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
  virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
  virtual void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
  virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
  
  // introduced virtual methods:
  virtual void draw(MYQCPPainter *painter);
  virtual void updateLayout();
  virtual void axisRemoved(MYQCPAxis *axis);
  virtual void legendRemoved(MYQCPLegend *legend);
  Q_SLOT virtual void processRectSelection(QRect rect, QMouseEvent *event);
  Q_SLOT virtual void processRectZoom(QRect rect, QMouseEvent *event);
  Q_SLOT virtual void processPointSelection(QMouseEvent *event);
  
  // non-virtual methods:
  bool registerPlottable(MYQCPAbstractPlottable *plottable);
  bool registerGraph(MYQCPGraph *graph);
  bool registerItem(MYQCPAbstractItem* item);
  void updateLayerIndices() const;
  MYQCPLayerable *layerableAt(const QPointF &pos, bool onlySelectable, QVariant *selectionDetails=0) const;
  QList<MYQCPLayerable*> layerableListAt(const QPointF &pos, bool onlySelectable, QList<QVariant> *selectionDetails=0) const;
  void drawBackground(MYQCPPainter *painter);
  void setupPaintBuffers();
  MYQCPAbstractPaintBuffer *createPaintBuffer();
  bool hasInvalidatedPaintBuffers();
  bool setupOpenGl();
  void freeOpenGl();
  
  friend class MYQCPLegend;
  friend class MYQCPAxis;
  friend class MYQCPLayer;
  friend class MYQCPAxisRect;
  friend class MYQCPAbstractPlottable;
  friend class MYQCPGraph;
  friend class MYQCPAbstractItem;
};
Q_DECLARE_METATYPE(QCustomPlot::LayerInsertMode)
Q_DECLARE_METATYPE(QCustomPlot::RefreshPriority)

/* end of 'src/core.h' */


/* including file 'src/plottable1d.h', size 4544                             */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCPPlottableInterface1D
{
public:
  virtual ~MYQCPPlottableInterface1D() {}
  // introduced pure virtual methods:
  virtual int dataCount() const = 0;
  virtual double dataMainKey(int index) const = 0;
  virtual double dataSortKey(int index) const = 0;
  virtual double dataMainValue(int index) const = 0;
  virtual MYQCPRange dataValueRange(int index) const = 0;
  virtual QPointF dataPixelPosition(int index) const = 0;
  virtual bool sortKeyIsMainKey() const = 0;
  virtual MYQCPDataSelection selectTestRect(const QRectF &rect, bool onlySelectable) const = 0;
  virtual int findBegin(double sortKey, bool expandedRange=true) const = 0;
  virtual int findEnd(double sortKey, bool expandedRange=true) const = 0;
};

template <class DataType>
class MYQCPAbstractPlottable1D : public MYQCPAbstractPlottable, public MYQCPPlottableInterface1D // no MYQCP_LIB_DECL, template class ends up in header (cpp included below)
{
  // No Q_OBJECT macro due to template class
  
public:
  MYQCPAbstractPlottable1D(MYQCPAxis *keyAxis, MYQCPAxis *valueAxis);
  virtual ~MYQCPAbstractPlottable1D();
  
  // virtual methods of 1d plottable interface:
  virtual int dataCount() const Q_DECL_OVERRIDE;
  virtual double dataMainKey(int index) const Q_DECL_OVERRIDE;
  virtual double dataSortKey(int index) const Q_DECL_OVERRIDE;
  virtual double dataMainValue(int index) const Q_DECL_OVERRIDE;
  virtual MYQCPRange dataValueRange(int index) const Q_DECL_OVERRIDE;
  virtual QPointF dataPixelPosition(int index) const Q_DECL_OVERRIDE;
  virtual bool sortKeyIsMainKey() const Q_DECL_OVERRIDE;
  virtual MYQCPDataSelection selectTestRect(const QRectF &rect, bool onlySelectable) const Q_DECL_OVERRIDE;
  virtual int findBegin(double sortKey, bool expandedRange=true) const Q_DECL_OVERRIDE;
  virtual int findEnd(double sortKey, bool expandedRange=true) const Q_DECL_OVERRIDE;
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  virtual MYQCPPlottableInterface1D *interface1D() Q_DECL_OVERRIDE { return this; }
  
protected:
  // property members:
  QSharedPointer<MYQCPDataContainer<DataType> > mDataContainer;
  
  // helpers for subclasses:
  void getDataSegments(QList<MYQCPDataRange> &selectedSegments, QList<MYQCPDataRange> &unselectedSegments) const;
  void drawPolyline(MYQCPPainter *painter, const QVector<QPointF> &lineData) const;

private:
  Q_DISABLE_COPY(MYQCPAbstractPlottable1D)
  
};

// include implementation in header since it is a class template:

/* including file 'src/plottable1d.cpp', size 22361                          */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// MYQCPPlottableInterface1D
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class MYQCPPlottableInterface1D
  \brief Defines an abstract interface for one-dimensional plottables

  This class contains only pure virtual methods which define a common interface to the data
  of one-dimensional plottables.

  For example, it is implemented by the template class \ref MYQCPAbstractPlottable1D (the preferred
  base class for one-dimensional plottables). So if you use that template class as base class of
  your one-dimensional plottable, you won't have to care about implementing the 1d interface
  yourself.

  If your plottable doesn't derive from \ref MYQCPAbstractPlottable1D but still wants to provide a 1d
  interface (e.g. like \ref MYQCPErrorBars does), you should inherit from both \ref
  MYQCPAbstractPlottable and \ref MYQCPPlottableInterface1D and accordingly reimplement the pure
  virtual methods of the 1d interface, matching your data container. Also, reimplement \ref
  MYQCPAbstractPlottable::interface1D to return the \c this pointer.

  If you have a \ref MYQCPAbstractPlottable pointer, you can check whether it implements this
  interface by calling \ref MYQCPAbstractPlottable::interface1D and testing it for a non-zero return
  value. If it indeed implements this interface, you may use it to access the plottable's data
  without needing to know the exact type of the plottable or its data point type.
*/

/* start documentation of pure virtual functions */

/*! \fn virtual int MYQCPPlottableInterface1D::dataCount() const = 0;
  
  Returns the number of data points of the plottable.
*/

/*! \fn virtual MYQCPDataSelection MYQCPPlottableInterface1D::selectTestRect(const QRectF &rect, bool onlySelectable) const = 0;
  
  Returns a data selection containing all the data points of this plottable which are contained (or
  hit by) \a rect. This is used mainly in the selection rect interaction for data selection (\ref
  dataselection "data selection mechanism").
  
  If \a onlySelectable is true, an empty MYQCPDataSelection is returned if this plottable is not
  selectable (i.e. if \ref MYQCPAbstractPlottable::setSelectable is \ref MYQCP::stNone).
  
  \note \a rect must be a normalized rect (positive or zero width and height). This is especially
  important when using the rect of \ref MYQCPSelectionRect::accepted, which is not necessarily
  normalized. Use <tt>QRect::normalized()</tt> when passing a rect which might not be normalized.
*/

/*! \fn virtual double MYQCPPlottableInterface1D::dataMainKey(int index) const = 0
  
  Returns the main key of the data point at the given \a index.
  
  What the main key is, is defined by the plottable's data type. See the \ref
  MYQCPdatacontainer-datatype "MYQCPDataContainer DataType" documentation for details about this naming
  convention.
*/

/*! \fn virtual double MYQCPPlottableInterface1D::dataSortKey(int index) const = 0
  
  Returns the sort key of the data point at the given \a index.
  
  What the sort key is, is defined by the plottable's data type. See the \ref
  MYQCPdatacontainer-datatype "MYQCPDataContainer DataType" documentation for details about this naming
  convention.
*/

/*! \fn virtual double MYQCPPlottableInterface1D::dataMainValue(int index) const = 0
  
  Returns the main value of the data point at the given \a index.
  
  What the main value is, is defined by the plottable's data type. See the \ref
  MYQCPdatacontainer-datatype "MYQCPDataContainer DataType" documentation for details about this naming
  convention.
*/

/*! \fn virtual MYQCPRange MYQCPPlottableInterface1D::dataValueRange(int index) const = 0
  
  Returns the value range of the data point at the given \a index.
  
  What the value range is, is defined by the plottable's data type. See the \ref
  MYQCPdatacontainer-datatype "MYQCPDataContainer DataType" documentation for details about this naming
  convention.
*/

/*! \fn virtual QPointF MYQCPPlottableInterface1D::dataPixelPosition(int index) const = 0

  Returns the pixel position on the widget surface at which the data point at the given \a index
  appears.

  Usually this corresponds to the point of \ref dataMainKey/\ref dataMainValue, in pixel
  coordinates. However, depending on the plottable, this might be a different apparent position
  than just a coord-to-pixel transform of those values. For example, \ref MYQCPBars apparent data
  values can be shifted depending on their stacking, bar grouping or configured base value.
*/

/*! \fn virtual bool MYQCPPlottableInterface1D::sortKeyIsMainKey() const = 0

  Returns whether the sort key (\ref dataSortKey) is identical to the main key (\ref dataMainKey).

  What the sort and main keys are, is defined by the plottable's data type. See the \ref
  MYQCPdatacontainer-datatype "MYQCPDataContainer DataType" documentation for details about this naming
  convention.
*/

/*! \fn virtual int MYQCPPlottableInterface1D::findBegin(double sortKey, bool expandedRange) const = 0

  Returns the index of the data point with a (sort-)key that is equal to, just below, or just above
  \a sortKey. If \a expandedRange is true, the data point just below \a sortKey will be considered,
  otherwise the one just above.

  This can be used in conjunction with \ref findEnd to iterate over data points within a given key
  range, including or excluding the bounding data points that are just beyond the specified range.

  If \a expandedRange is true but there are no data points below \a sortKey, 0 is returned.

  If the container is empty, returns 0 (in that case, \ref findEnd will also return 0, so a loop
  using these methods will not iterate over the index 0).

  \see findEnd, MYQCPDataContainer::findBegin
*/

/*! \fn virtual int MYQCPPlottableInterface1D::findEnd(double sortKey, bool expandedRange) const = 0

  Returns the index one after the data point with a (sort-)key that is equal to, just above, or
  just below \a sortKey. If \a expandedRange is true, the data point just above \a sortKey will be
  considered, otherwise the one just below.

  This can be used in conjunction with \ref findBegin to iterate over data points within a given
  key range, including the bounding data points that are just below and above the specified range.

  If \a expandedRange is true but there are no data points above \a sortKey, the index just above the
  highest data point is returned.

  If the container is empty, returns 0.

  \see findBegin, MYQCPDataContainer::findEnd
*/

/* end documentation of pure virtual functions */


////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// MYQCPAbstractPlottable1D
////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \class MYQCPAbstractPlottable1D
  \brief A template base class for plottables with one-dimensional data

  This template class derives from \ref MYQCPAbstractPlottable and from the abstract interface \ref
  MYQCPPlottableInterface1D. It serves as a base class for all one-dimensional data (i.e. data with
  one key dimension), such as \ref MYQCPGraph and MYQCPCurve.

  The template parameter \a DataType is the type of the data points of this plottable (e.g. \ref
  MYQCPGraphData or \ref MYQCPCurveData). The main purpose of this base class is to provide the member
  \a mDataContainer (a shared pointer to a \ref MYQCPDataContainer "MYQCPDataContainer<DataType>") and
  implement the according virtual methods of the \ref MYQCPPlottableInterface1D, such that most
  subclassed plottables don't need to worry about this anymore.

  Further, it provides a convenience method for retrieving selected/unselected data segments via
  \ref getDataSegments. This is useful when subclasses implement their \ref draw method and need to
  draw selected segments with a different pen/brush than unselected segments (also see \ref
  MYQCPSelectionDecorator).

  This class implements basic functionality of \ref MYQCPAbstractPlottable::selectTest and \ref
  MYQCPPlottableInterface1D::selectTestRect, assuming point-like data points, based on the 1D data
  interface. In spite of that, most plottable subclasses will want to reimplement those methods
  again, to provide a more accurate hit test based on their specific data visualization geometry.
*/

/* start documentation of inline functions */

/*! \fn MYQCPPlottableInterface1D *MYQCPAbstractPlottable1D::interface1D()
  
  Returns a \ref MYQCPPlottableInterface1D pointer to this plottable, providing access to its 1D
  interface.
  
  \seebaseclassmethod
*/

/* end documentation of inline functions */

/*!
  Forwards \a keyAxis and \a valueAxis to the \ref MYQCPAbstractPlottable::MYQCPAbstractPlottable
  "MYQCPAbstractPlottable" constructor and allocates the \a mDataContainer.
*/
template <class DataType>
MYQCPAbstractPlottable1D<DataType>::MYQCPAbstractPlottable1D(MYQCPAxis *keyAxis, MYQCPAxis *valueAxis) :
  MYQCPAbstractPlottable(keyAxis, valueAxis),
  mDataContainer(new MYQCPDataContainer<DataType>)
{
}

template <class DataType>
MYQCPAbstractPlottable1D<DataType>::~MYQCPAbstractPlottable1D()
{
}

/*!
  \copydoc MYQCPPlottableInterface1D::dataCount
*/
template <class DataType>
int MYQCPAbstractPlottable1D<DataType>::dataCount() const
{
  return mDataContainer->size();
}

/*!
  \copydoc MYQCPPlottableInterface1D::dataMainKey
*/
template <class DataType>
double MYQCPAbstractPlottable1D<DataType>::dataMainKey(int index) const
{
  if (index >= 0 && index < mDataContainer->size())
  {
    return (mDataContainer->constBegin()+index)->mainKey();
  } else
  {
    qDebug() << Q_FUNC_INFO << "Index out of bounds" << index;
    return 0;
  }
}

/*!
  \copydoc MYQCPPlottableInterface1D::dataSortKey
*/
template <class DataType>
double MYQCPAbstractPlottable1D<DataType>::dataSortKey(int index) const
{
  if (index >= 0 && index < mDataContainer->size())
  {
    return (mDataContainer->constBegin()+index)->sortKey();
  } else
  {
    qDebug() << Q_FUNC_INFO << "Index out of bounds" << index;
    return 0;
  }
}

/*!
  \copydoc MYQCPPlottableInterface1D::dataMainValue
*/
template <class DataType>
double MYQCPAbstractPlottable1D<DataType>::dataMainValue(int index) const
{
  if (index >= 0 && index < mDataContainer->size())
  {
    return (mDataContainer->constBegin()+index)->mainValue();
  } else
  {
    qDebug() << Q_FUNC_INFO << "Index out of bounds" << index;
    return 0;
  }
}

/*!
  \copydoc MYQCPPlottableInterface1D::dataValueRange
*/
template <class DataType>
MYQCPRange MYQCPAbstractPlottable1D<DataType>::dataValueRange(int index) const
{
  if (index >= 0 && index < mDataContainer->size())
  {
    return (mDataContainer->constBegin()+index)->valueRange();
  } else
  {
    qDebug() << Q_FUNC_INFO << "Index out of bounds" << index;
    return MYQCPRange(0, 0);
  }
}

/*!
  \copydoc MYQCPPlottableInterface1D::dataPixelPosition
*/
template <class DataType>
QPointF MYQCPAbstractPlottable1D<DataType>::dataPixelPosition(int index) const
{
  if (index >= 0 && index < mDataContainer->size())
  {
    const typename MYQCPDataContainer<DataType>::const_iterator it = mDataContainer->constBegin()+index;
    return coordsToPixels(it->mainKey(), it->mainValue());
  } else
  {
    qDebug() << Q_FUNC_INFO << "Index out of bounds" << index;
    return QPointF();
  }
}

/*!
  \copydoc MYQCPPlottableInterface1D::sortKeyIsMainKey
*/
template <class DataType>
bool MYQCPAbstractPlottable1D<DataType>::sortKeyIsMainKey() const
{
  return DataType::sortKeyIsMainKey();
}

/*!
  Implements a rect-selection algorithm assuming the data (accessed via the 1D data interface) is
  point-like. Most subclasses will want to reimplement this method again, to provide a more
  accurate hit test based on the true data visualization geometry.

  \seebaseclassmethod
*/
template <class DataType>
MYQCPDataSelection MYQCPAbstractPlottable1D<DataType>::selectTestRect(const QRectF &rect, bool onlySelectable) const
{
  MYQCPDataSelection result;
  if ((onlySelectable && mSelectable == MYQCP::stNone) || mDataContainer->isEmpty())
    return result;
  if (!mKeyAxis || !mValueAxis)
    return result;
  
  // convert rect given in pixels to ranges given in plot coordinates:
  double key1, value1, key2, value2;
  pixelsToCoords(rect.topLeft(), key1, value1);
  pixelsToCoords(rect.bottomRight(), key2, value2);
  MYQCPRange keyRange(key1, key2); // MYQCPRange normalizes internally so we don't have to care about whether key1 < key2
  MYQCPRange valueRange(value1, value2);
  typename MYQCPDataContainer<DataType>::const_iterator begin = mDataContainer->constBegin();
  typename MYQCPDataContainer<DataType>::const_iterator end = mDataContainer->constEnd();
  if (DataType::sortKeyIsMainKey()) // we can assume that data is sorted by main key, so can reduce the searched key interval:
  {
    begin = mDataContainer->findBegin(keyRange.lower, false);
    end = mDataContainer->findEnd(keyRange.upper, false);
  }
  if (begin == end)
    return result;
  
  int currentSegmentBegin = -1; // -1 means we're currently not in a segment that's contained in rect
  for (typename MYQCPDataContainer<DataType>::const_iterator it=begin; it!=end; ++it)
  {
    if (currentSegmentBegin == -1)
    {
      if (valueRange.contains(it->mainValue()) && keyRange.contains(it->mainKey())) // start segment
        currentSegmentBegin = it-mDataContainer->constBegin();
    } else if (!valueRange.contains(it->mainValue()) || !keyRange.contains(it->mainKey())) // segment just ended
    {
      result.addDataRange(MYQCPDataRange(currentSegmentBegin, it-mDataContainer->constBegin()), false);
      currentSegmentBegin = -1;
    }
  }
  // process potential last segment:
  if (currentSegmentBegin != -1)
    result.addDataRange(MYQCPDataRange(currentSegmentBegin, end-mDataContainer->constBegin()), false);
  
  result.simplify();
  return result;
}

/*!
  \copydoc MYQCPPlottableInterface1D::findBegin
*/
template <class DataType>
int MYQCPAbstractPlottable1D<DataType>::findBegin(double sortKey, bool expandedRange) const
{
  return mDataContainer->findBegin(sortKey, expandedRange)-mDataContainer->constBegin();
}

/*!
  \copydoc MYQCPPlottableInterface1D::findEnd
*/
template <class DataType>
int MYQCPAbstractPlottable1D<DataType>::findEnd(double sortKey, bool expandedRange) const
{
  return mDataContainer->findEnd(sortKey, expandedRange)-mDataContainer->constBegin();
}

/*!
  Implements a point-selection algorithm assuming the data (accessed via the 1D data interface) is
  point-like. Most subclasses will want to reimplement this method again, to provide a more
  accurate hit test based on the true data visualization geometry.

  If \a details is not 0, it will be set to a \ref MYQCPDataSelection, describing the closest data point
  to \a pos.
  
  \seebaseclassmethod
*/
template <class DataType>
double MYQCPAbstractPlottable1D<DataType>::selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const
{
  if ((onlySelectable && mSelectable == MYQCP::stNone) || mDataContainer->isEmpty())
    return -1;
  if (!mKeyAxis || !mValueAxis)
    return -1;
  
  MYQCPDataSelection selectionResult;
  double minDistSqr = (std::numeric_limits<double>::max)();
  int minDistIndex = mDataContainer->size();
  
  typename MYQCPDataContainer<DataType>::const_iterator begin = mDataContainer->constBegin();
  typename MYQCPDataContainer<DataType>::const_iterator end = mDataContainer->constEnd();
  if (DataType::sortKeyIsMainKey()) // we can assume that data is sorted by main key, so can reduce the searched key interval:
  {
    // determine which key range comes into question, taking selection tolerance around pos into account:
    double posKeyMin, posKeyMax, dummy;
    pixelsToCoords(pos-QPointF(mParentPlot->selectionTolerance(), mParentPlot->selectionTolerance()), posKeyMin, dummy);
    pixelsToCoords(pos+QPointF(mParentPlot->selectionTolerance(), mParentPlot->selectionTolerance()), posKeyMax, dummy);
    if (posKeyMin > posKeyMax)
      qSwap(posKeyMin, posKeyMax);
    begin = mDataContainer->findBegin(posKeyMin, true);
    end = mDataContainer->findEnd(posKeyMax, true);
  }
  if (begin == end)
    return -1;
  MYQCPRange keyRange(mKeyAxis->range());
  MYQCPRange valueRange(mValueAxis->range());
  for (typename MYQCPDataContainer<DataType>::const_iterator it=begin; it!=end; ++it)
  {
    const double mainKey = it->mainKey();
    const double mainValue = it->mainValue();
    if (keyRange.contains(mainKey) && valueRange.contains(mainValue)) // make sure data point is inside visible range, for speedup in cases where sort key isn't main key and we iterate over all points
    {
      const double currentDistSqr = MYQCPVector2D(coordsToPixels(mainKey, mainValue)-pos).lengthSquared();
      if (currentDistSqr < minDistSqr)
      {
        minDistSqr = currentDistSqr;
        minDistIndex = it-mDataContainer->constBegin();
      }
    }
  }
  if (minDistIndex != mDataContainer->size())
    selectionResult.addDataRange(MYQCPDataRange(minDistIndex, minDistIndex+1), false);
  
  selectionResult.simplify();
  if (details)
    details->setValue(selectionResult);
  return qSqrt(minDistSqr);
}

/*!
  Splits all data into selected and unselected segments and outputs them via \a selectedSegments
  and \a unselectedSegments, respectively.

  This is useful when subclasses implement their \ref draw method and need to draw selected
  segments with a different pen/brush than unselected segments (also see \ref
  MYQCPSelectionDecorator).

  \see setSelection
*/
template <class DataType>
void MYQCPAbstractPlottable1D<DataType>::getDataSegments(QList<MYQCPDataRange> &selectedSegments, QList<MYQCPDataRange> &unselectedSegments) const
{
  selectedSegments.clear();
  unselectedSegments.clear();
  if (mSelectable == MYQCP::stWhole) // stWhole selection type draws the entire plottable with selected style if mSelection isn't empty
  {
    if (selected())
      selectedSegments << MYQCPDataRange(0, dataCount());
    else
      unselectedSegments << MYQCPDataRange(0, dataCount());
  } else
  {
    MYQCPDataSelection sel(selection());
    sel.simplify();
    selectedSegments = sel.dataRanges();
    unselectedSegments = sel.inverse(MYQCPDataRange(0, dataCount())).dataRanges();
  }
}

/*!
  A helper method which draws a line with the passed \a painter, according to the pixel data in \a
  lineData. NaN points create gaps in the line, as expected from QCustomPlot's plottables (this is
  the main difference to QPainter's regular drawPolyline, which handles NaNs by lagging or
  crashing).

  Further it uses a faster line drawing technique based on \ref MYQCPPainter::drawLine rather than \c
  QPainter::drawPolyline if the configured \ref QCustomPlot::setPlottingHints() and \a painter
  style allows.
*/
template <class DataType>
void MYQCPAbstractPlottable1D<DataType>::drawPolyline(MYQCPPainter *painter, const QVector<QPointF> &lineData) const
{
  // if drawing solid line and not in PDF, use much faster line drawing instead of polyline:
  if (mParentPlot->plottingHints().testFlag(MYQCP::phFastPolylines) &&
      painter->pen().style() == Qt::SolidLine &&
      !painter->modes().testFlag(MYQCPPainter::pmVectorized) &&
      !painter->modes().testFlag(MYQCPPainter::pmNoCaching))
  {
    int i = 0;
    bool lastIsNan = false;
    const int lineDataSize = lineData.size();
    while (i < lineDataSize && (qIsNaN(lineData.at(i).y()) || qIsNaN(lineData.at(i).x()))) // make sure first point is not NaN
      ++i;
    ++i; // because drawing works in 1 point retrospect
    while (i < lineDataSize)
    {
      if (!qIsNaN(lineData.at(i).y()) && !qIsNaN(lineData.at(i).x())) // NaNs create a gap in the line
      {
        if (!lastIsNan)
          painter->drawLine(lineData.at(i-1), lineData.at(i));
        else
          lastIsNan = false;
      } else
        lastIsNan = true;
      ++i;
    }
  } else
  {
    int segmentStart = 0;
    int i = 0;
    const int lineDataSize = lineData.size();
    while (i < lineDataSize)
    {
      if (qIsNaN(lineData.at(i).y()) || qIsNaN(lineData.at(i).x()) || qIsInf(lineData.at(i).y())) // NaNs create a gap in the line. Also filter Infs which make drawPolyline block
      {
        painter->drawPolyline(lineData.constData()+segmentStart, i-segmentStart); // i, because we don't want to include the current NaN point
        segmentStart = i+1;
      }
      ++i;
    }
    // draw last segment:
    painter->drawPolyline(lineData.constData()+segmentStart, lineDataSize-segmentStart);
  }
}
/* end of 'src/plottable1d.cpp' */


/* end of 'src/plottable1d.h' */


/* including file 'src/colorgradient.h', size 6243                           */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPColorGradient
{
  Q_GADGET
public:
  /*!
    Defines the color spaces in which color interpolation between gradient stops can be performed.
    
    \see setColorInterpolation
  */
  enum ColorInterpolation { ciRGB  ///< Color channels red, green and blue are linearly interpolated
                            ,ciHSV ///< Color channels hue, saturation and value are linearly interpolated (The hue is interpolated over the shortest angle distance)
                          };
  Q_ENUMS(ColorInterpolation)
  
  /*!
    Defines the available presets that can be loaded with \ref loadPreset. See the documentation
    there for an image of the presets.
  */
  enum GradientPreset { gpGrayscale  ///< Continuous lightness from black to white (suited for non-biased data representation)
                        ,gpHot       ///< Continuous lightness from black over firey colors to white (suited for non-biased data representation)
                        ,gpCold      ///< Continuous lightness from black over icey colors to white (suited for non-biased data representation)
                        ,gpNight     ///< Continuous lightness from black over weak blueish colors to white (suited for non-biased data representation)
                        ,gpCandy     ///< Blue over pink to white
                        ,gpGeography ///< Colors suitable to represent different elevations on geographical maps
                        ,gpIon       ///< Half hue spectrum from black over purple to blue and finally green (creates banding illusion but allows more precise magnitude estimates)
                        ,gpThermal   ///< Colors suitable for thermal imaging, ranging from dark blue over purple to orange, yellow and white
                        ,gpPolar     ///< Colors suitable to emphasize polarity around the center, with blue for negative, black in the middle and red for positive values
                        ,gpSpectrum  ///< An approximation of the visible light spectrum (creates banding illusion but allows more precise magnitude estimates)
                        ,gpJet       ///< Hue variation similar to a spectrum, often used in numerical visualization (creates banding illusion but allows more precise magnitude estimates)
                        ,gpHues      ///< Full hue cycle, with highest and lowest color red (suitable for periodic data, such as angles and phases, see \ref setPeriodic)
                      };
  Q_ENUMS(GradientPreset)
  
  MYQCPColorGradient();
  MYQCPColorGradient(GradientPreset preset);
  bool operator==(const MYQCPColorGradient &other) const;
  bool operator!=(const MYQCPColorGradient &other) const { return !(*this == other); }
  
  // getters:
  int levelCount() const { return mLevelCount; }
  QMap<double, QColor> colorStops() const { return mColorStops; }
  ColorInterpolation colorInterpolation() const { return mColorInterpolation; }
  bool periodic() const { return mPeriodic; }
  
  // setters:
  void setLevelCount(int n);
  void setColorStops(const QMap<double, QColor> &colorStops);
  void setColorStopAt(double position, const QColor &color);
  void setColorInterpolation(ColorInterpolation interpolation);
  void setPeriodic(bool enabled);
  
  // non-property methods:
  void colorize(const double *data, const MYQCPRange &range, QRgb *scanLine, int n, int dataIndexFactor=1, bool logarithmic=false);
  void colorize(const double *data, const unsigned char *alpha, const MYQCPRange &range, QRgb *scanLine, int n, int dataIndexFactor=1, bool logarithmic=false);
  QRgb color(double position, const MYQCPRange &range, bool logarithmic=false);
  void loadPreset(GradientPreset preset);
  void clearColorStops();
  MYQCPColorGradient inverted() const;
  
protected:
  // property members:
  int mLevelCount;
  QMap<double, QColor> mColorStops;
  ColorInterpolation mColorInterpolation;
  bool mPeriodic;
  
  // non-property members:
  QVector<QRgb> mColorBuffer; // have colors premultiplied with alpha (for usage with QImage::Format_ARGB32_Premultiplied)
  bool mColorBufferInvalidated;
  
  // non-virtual methods:
  bool stopsUseAlpha() const;
  void updateColorBuffer();
};
Q_DECLARE_METATYPE(MYQCPColorGradient::ColorInterpolation)
Q_DECLARE_METATYPE(MYQCPColorGradient::GradientPreset)

/* end of 'src/colorgradient.h' */


/* including file 'src/selectiondecorator-bracket.h', size 4442              */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPSelectionDecoratorBracket : public MYQCPSelectionDecorator
{
  Q_GADGET
public:
  
  /*!
    Defines which shape is drawn at the boundaries of selected data ranges.
    
    Some of the bracket styles further allow specifying a height and/or width, see \ref
    setBracketHeight and \ref setBracketWidth.
  */
  enum BracketStyle { bsSquareBracket ///< A square bracket is drawn.
                      ,bsHalfEllipse   ///< A half ellipse is drawn. The size of the ellipse is given by the bracket width/height properties.
                      ,bsEllipse       ///< An ellipse is drawn. The size of the ellipse is given by the bracket width/height properties.
                      ,bsPlus         ///< A plus is drawn.
                      ,bsUserStyle    ///< Start custom bracket styles at this index when subclassing and reimplementing \ref drawBracket.
  };
  Q_ENUMS(BracketStyle)
  
  MYQCPSelectionDecoratorBracket();
  virtual ~MYQCPSelectionDecoratorBracket();
  
  // getters:
  QPen bracketPen() const { return mBracketPen; }
  QBrush bracketBrush() const { return mBracketBrush; }
  int bracketWidth() const { return mBracketWidth; }
  int bracketHeight() const { return mBracketHeight; }
  BracketStyle bracketStyle() const { return mBracketStyle; }
  bool tangentToData() const { return mTangentToData; }
  int tangentAverage() const { return mTangentAverage; }
  
  // setters:
  void setBracketPen(const QPen &pen);
  void setBracketBrush(const QBrush &brush);
  void setBracketWidth(int width);
  void setBracketHeight(int height);
  void setBracketStyle(BracketStyle style);
  void setTangentToData(bool enabled);
  void setTangentAverage(int pointCount);
  
  // introduced virtual methods:
  virtual void drawBracket(MYQCPPainter *painter, int direction) const;
  
  // virtual methods:
  virtual void drawDecoration(MYQCPPainter *painter, MYQCPDataSelection selection) Q_DECL_OVERRIDE;
  
protected:
  // property members:
  QPen mBracketPen;
  QBrush mBracketBrush;
  int mBracketWidth;
  int mBracketHeight;
  BracketStyle mBracketStyle;
  bool mTangentToData;
  int mTangentAverage;
  
  // non-virtual methods:
  double getTangentAngle(const MYQCPPlottableInterface1D *interface1d, int dataIndex, int direction) const;
  QPointF getPixelCoordinates(const MYQCPPlottableInterface1D *interface1d, int dataIndex) const;
  
};
Q_DECLARE_METATYPE(MYQCPSelectionDecoratorBracket::BracketStyle)

/* end of 'src/selectiondecorator-bracket.h' */


/* including file 'src/layoutelements/layoutelement-axisrect.h', size 7507   */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPAxisRect : public MYQCPLayoutElement
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPixmap background READ background WRITE setBackground)
  Q_PROPERTY(bool backgroundScaled READ backgroundScaled WRITE setBackgroundScaled)
  Q_PROPERTY(Qt::AspectRatioMode backgroundScaledMode READ backgroundScaledMode WRITE setBackgroundScaledMode)
  Q_PROPERTY(Qt::Orientations rangeDrag READ rangeDrag WRITE setRangeDrag)
  Q_PROPERTY(Qt::Orientations rangeZoom READ rangeZoom WRITE setRangeZoom)
  /// \endcond
public:
  explicit MYQCPAxisRect(QCustomPlot *parentPlot, bool setupDefaultAxes=true);
  virtual ~MYQCPAxisRect();
  
  // getters:
  QPixmap background() const { return mBackgroundPixmap; }
  QBrush backgroundBrush() const { return mBackgroundBrush; }
  bool backgroundScaled() const { return mBackgroundScaled; }
  Qt::AspectRatioMode backgroundScaledMode() const { return mBackgroundScaledMode; }
  Qt::Orientations rangeDrag() const { return mRangeDrag; }
  Qt::Orientations rangeZoom() const { return mRangeZoom; }
  MYQCPAxis *rangeDragAxis(Qt::Orientation orientation);
  MYQCPAxis *rangeZoomAxis(Qt::Orientation orientation);
  QList<MYQCPAxis*> rangeDragAxes(Qt::Orientation orientation);
  QList<MYQCPAxis*> rangeZoomAxes(Qt::Orientation orientation);
  double rangeZoomFactor(Qt::Orientation orientation);
  
  // setters:
  void setBackground(const QPixmap &pm);
  void setBackground(const QPixmap &pm, bool scaled, Qt::AspectRatioMode mode=Qt::KeepAspectRatioByExpanding);
  void setBackground(const QBrush &brush);
  void setBackgroundScaled(bool scaled);
  void setBackgroundScaledMode(Qt::AspectRatioMode mode);
  void setRangeDrag(Qt::Orientations orientations);
  void setRangeZoom(Qt::Orientations orientations);
  void setRangeDragAxes(MYQCPAxis *horizontal, MYQCPAxis *vertical);
  void setRangeDragAxes(QList<MYQCPAxis*> axes);
  void setRangeDragAxes(QList<MYQCPAxis*> horizontal, QList<MYQCPAxis*> vertical);
  void setRangeZoomAxes(MYQCPAxis *horizontal, MYQCPAxis *vertical);
  void setRangeZoomAxes(QList<MYQCPAxis*> axes);
  void setRangeZoomAxes(QList<MYQCPAxis*> horizontal, QList<MYQCPAxis*> vertical);
  void setRangeZoomFactor(double horizontalFactor, double verticalFactor);
  void setRangeZoomFactor(double factor);
  
  // non-property methods:
  int axisCount(MYQCPAxis::AxisType type) const;
  MYQCPAxis *axis(MYQCPAxis::AxisType type, int index=0) const;
  QList<MYQCPAxis*> axes(MYQCPAxis::AxisTypes types) const;
  QList<MYQCPAxis*> axes() const;
  MYQCPAxis *addAxis(MYQCPAxis::AxisType type, MYQCPAxis *axis=0);
  QList<MYQCPAxis*> addAxes(MYQCPAxis::AxisTypes types);
  bool removeAxis(MYQCPAxis *axis);
  MYQCPLayoutInset *insetLayout() const { return mInsetLayout; }
  
  void zoom(const QRectF &pixelRect);
  void zoom(const QRectF &pixelRect, const QList<MYQCPAxis*> &affectedAxes);
  void setupFullAxesBox(bool connectRanges=false);
  QList<MYQCPAbstractPlottable*> plottables() const;
  QList<MYQCPGraph*> graphs() const;
  QList<MYQCPAbstractItem*> items() const;
  
  // read-only interface imitating a QRect:
  int left() const { return mRect.left(); }
  int right() const { return mRect.right(); }
  int top() const { return mRect.top(); }
  int bottom() const { return mRect.bottom(); }
  int width() const { return mRect.width(); }
  int height() const { return mRect.height(); }
  QSize size() const { return mRect.size(); }
  QPoint topLeft() const { return mRect.topLeft(); }
  QPoint topRight() const { return mRect.topRight(); }
  QPoint bottomLeft() const { return mRect.bottomLeft(); }
  QPoint bottomRight() const { return mRect.bottomRight(); }
  QPoint center() const { return mRect.center(); }
  
  // reimplemented virtual methods:
  virtual void update(UpdatePhase phase) Q_DECL_OVERRIDE;
  virtual QList<MYQCPLayoutElement*> elements(bool recursive) const Q_DECL_OVERRIDE;

protected:
  // property members:
  QBrush mBackgroundBrush;
  QPixmap mBackgroundPixmap;
  QPixmap mScaledBackgroundPixmap;
  bool mBackgroundScaled;
  Qt::AspectRatioMode mBackgroundScaledMode;
  MYQCPLayoutInset *mInsetLayout;
  Qt::Orientations mRangeDrag, mRangeZoom;
  QList<QPointer<MYQCPAxis> > mRangeDragHorzAxis, mRangeDragVertAxis;
  QList<QPointer<MYQCPAxis> > mRangeZoomHorzAxis, mRangeZoomVertAxis;
  double mRangeZoomFactorHorz, mRangeZoomFactorVert;
  
  // non-property members:
  QList<MYQCPRange> mDragStartHorzRange, mDragStartVertRange;
  MYQCP::AntialiasedElements mAADragBackup, mNotAADragBackup;
  bool mDragging;
  QHash<MYQCPAxis::AxisType, QList<MYQCPAxis*> > mAxes;
  
  // reimplemented virtual methods:
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const Q_DECL_OVERRIDE;
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual int calculateAutoMargin(MYQCP::MarginSide side) Q_DECL_OVERRIDE;
  virtual void layoutChanged() Q_DECL_OVERRIDE;
  // events:
  virtual void mousePressEvent(QMouseEvent *event, const QVariant &details) Q_DECL_OVERRIDE;
  virtual void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos) Q_DECL_OVERRIDE;
  virtual void mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos) Q_DECL_OVERRIDE;
  virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
  
  // non-property methods:
  void drawBackground(MYQCPPainter *painter);
  void updateAxesOffset(MYQCPAxis::AxisType type);
  
private:
  Q_DISABLE_COPY(MYQCPAxisRect)
  
  friend class QCustomPlot;
};


/* end of 'src/layoutelements/layoutelement-axisrect.h' */


/* including file 'src/layoutelements/layoutelement-legend.h', size 10397    */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPAbstractLegendItem : public MYQCPLayoutElement
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(MYQCPLegend* parentLegend READ parentLegend)
  Q_PROPERTY(QFont font READ font WRITE setFont)
  Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)
  Q_PROPERTY(QFont selectedFont READ selectedFont WRITE setSelectedFont)
  Q_PROPERTY(QColor selectedTextColor READ selectedTextColor WRITE setSelectedTextColor)
  Q_PROPERTY(bool selectable READ selectable WRITE setSelectable NOTIFY selectionChanged)
  Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectableChanged)
  /// \endcond
public:
  explicit MYQCPAbstractLegendItem(MYQCPLegend *parent);
  
  // getters:
  MYQCPLegend *parentLegend() const { return mParentLegend; }
  QFont font() const { return mFont; }
  QColor textColor() const { return mTextColor; }
  QFont selectedFont() const { return mSelectedFont; }
  QColor selectedTextColor() const { return mSelectedTextColor; }
  bool selectable() const { return mSelectable; }
  bool selected() const { return mSelected; }
  
  // setters:
  void setFont(const QFont &font);
  void setTextColor(const QColor &color);
  void setSelectedFont(const QFont &font);
  void setSelectedTextColor(const QColor &color);
  Q_SLOT void setSelectable(bool selectable);
  Q_SLOT void setSelected(bool selected);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  
signals:
  void selectionChanged(bool selected);
  void selectableChanged(bool selectable);
  
protected:
  // property members:
  MYQCPLegend *mParentLegend;
  QFont mFont;
  QColor mTextColor;
  QFont mSelectedFont;
  QColor mSelectedTextColor;
  bool mSelectable, mSelected;
  
  // reimplemented virtual methods:
  virtual MYQCP::Interaction selectionCategory() const Q_DECL_OVERRIDE;
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const Q_DECL_OVERRIDE;
  virtual QRect clipRect() const Q_DECL_OVERRIDE;
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE = 0;
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged) Q_DECL_OVERRIDE;
  virtual void deselectEvent(bool *selectionStateChanged) Q_DECL_OVERRIDE;
  
private:
  Q_DISABLE_COPY(MYQCPAbstractLegendItem)
  
  friend class MYQCPLegend;
};


class MYQCP_LIB_DECL MYQCPPlottableLegendItem : public MYQCPAbstractLegendItem
{
  Q_OBJECT
public:
  MYQCPPlottableLegendItem(MYQCPLegend *parent, MYQCPAbstractPlottable *plottable);
  
  // getters:
  MYQCPAbstractPlottable *plottable() { return mPlottable; }
  
protected:
  // property members:
  MYQCPAbstractPlottable *mPlottable;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual QSize minimumOuterSizeHint() const Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  QPen getIconBorderPen() const;
  QColor getTextColor() const;
  QFont getFont() const;
};


class MYQCP_LIB_DECL MYQCPLegend : public MYQCPLayoutGrid
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen borderPen READ borderPen WRITE setBorderPen)
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
  Q_PROPERTY(QFont font READ font WRITE setFont)
  Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)
  Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
  Q_PROPERTY(int iconTextPadding READ iconTextPadding WRITE setIconTextPadding)
  Q_PROPERTY(QPen iconBorderPen READ iconBorderPen WRITE setIconBorderPen)
  Q_PROPERTY(SelectableParts selectableParts READ selectableParts WRITE setSelectableParts NOTIFY selectionChanged)
  Q_PROPERTY(SelectableParts selectedParts READ selectedParts WRITE setSelectedParts NOTIFY selectableChanged)
  Q_PROPERTY(QPen selectedBorderPen READ selectedBorderPen WRITE setSelectedBorderPen)
  Q_PROPERTY(QPen selectedIconBorderPen READ selectedIconBorderPen WRITE setSelectedIconBorderPen)
  Q_PROPERTY(QBrush selectedBrush READ selectedBrush WRITE setSelectedBrush)
  Q_PROPERTY(QFont selectedFont READ selectedFont WRITE setSelectedFont)
  Q_PROPERTY(QColor selectedTextColor READ selectedTextColor WRITE setSelectedTextColor)
  /// \endcond
public:
  /*!
    Defines the selectable parts of a legend
    
    \see setSelectedParts, setSelectableParts
  */
  enum SelectablePart { spNone        = 0x000 ///< <tt>0x000</tt> None
                        ,spLegendBox  = 0x001 ///< <tt>0x001</tt> The legend box (frame)
                        ,spItems      = 0x002 ///< <tt>0x002</tt> Legend items individually (see \ref selectedItems)
                      };
  Q_ENUMS(SelectablePart)
  Q_FLAGS(SelectableParts)
  Q_DECLARE_FLAGS(SelectableParts, SelectablePart)
  
  explicit MYQCPLegend();
  virtual ~MYQCPLegend();
  
  // getters:
  QPen borderPen() const { return mBorderPen; }
  QBrush brush() const { return mBrush; }
  QFont font() const { return mFont; }
  QColor textColor() const { return mTextColor; }
  QSize iconSize() const { return mIconSize; }
  int iconTextPadding() const { return mIconTextPadding; }
  QPen iconBorderPen() const { return mIconBorderPen; }
  SelectableParts selectableParts() const { return mSelectableParts; }
  SelectableParts selectedParts() const;
  QPen selectedBorderPen() const { return mSelectedBorderPen; }
  QPen selectedIconBorderPen() const { return mSelectedIconBorderPen; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  QFont selectedFont() const { return mSelectedFont; }
  QColor selectedTextColor() const { return mSelectedTextColor; }
  
  // setters:
  void setBorderPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setFont(const QFont &font);
  void setTextColor(const QColor &color);
  void setIconSize(const QSize &size);
  void setIconSize(int width, int height);
  void setIconTextPadding(int padding);
  void setIconBorderPen(const QPen &pen);
  Q_SLOT void setSelectableParts(const SelectableParts &selectableParts);
  Q_SLOT void setSelectedParts(const SelectableParts &selectedParts);
  void setSelectedBorderPen(const QPen &pen);
  void setSelectedIconBorderPen(const QPen &pen);
  void setSelectedBrush(const QBrush &brush);
  void setSelectedFont(const QFont &font);
  void setSelectedTextColor(const QColor &color);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  MYQCPAbstractLegendItem *item(int index) const;
  MYQCPPlottableLegendItem *itemWithPlottable(const MYQCPAbstractPlottable *plottable) const;
  int itemCount() const;
  bool hasItem(MYQCPAbstractLegendItem *item) const;
  bool hasItemWithPlottable(const MYQCPAbstractPlottable *plottable) const;
  bool addItem(MYQCPAbstractLegendItem *item);
  bool removeItem(int index);
  bool removeItem(MYQCPAbstractLegendItem *item);
  void clearItems();
  QList<MYQCPAbstractLegendItem*> selectedItems() const;
  
signals:
  void selectionChanged(MYQCPLegend::SelectableParts parts);
  void selectableChanged(MYQCPLegend::SelectableParts parts);
  
protected:
  // property members:
  QPen mBorderPen, mIconBorderPen;
  QBrush mBrush;
  QFont mFont;
  QColor mTextColor;
  QSize mIconSize;
  int mIconTextPadding;
  SelectableParts mSelectedParts, mSelectableParts;
  QPen mSelectedBorderPen, mSelectedIconBorderPen;
  QBrush mSelectedBrush;
  QFont mSelectedFont;
  QColor mSelectedTextColor;
  
  // reimplemented virtual methods:
  virtual void parentPlotInitialized(QCustomPlot *parentPlot) Q_DECL_OVERRIDE;
  virtual MYQCP::Interaction selectionCategory() const Q_DECL_OVERRIDE;
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const Q_DECL_OVERRIDE;
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged) Q_DECL_OVERRIDE;
  virtual void deselectEvent(bool *selectionStateChanged) Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  QPen getBorderPen() const;
  QBrush getBrush() const;
  
private:
  Q_DISABLE_COPY(MYQCPLegend)
  
  friend class QCustomPlot;
  friend class MYQCPAbstractLegendItem;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(MYQCPLegend::SelectableParts)
Q_DECLARE_METATYPE(MYQCPLegend::SelectablePart)

/* end of 'src/layoutelements/layoutelement-legend.h' */


/* including file 'src/layoutelements/layoutelement-textelement.h', size 5353 */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200  */

class MYQCP_LIB_DECL MYQCPTextElement : public MYQCPLayoutElement
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QString text READ text WRITE setText)
  Q_PROPERTY(QFont font READ font WRITE setFont)
  Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)
  Q_PROPERTY(QFont selectedFont READ selectedFont WRITE setSelectedFont)
  Q_PROPERTY(QColor selectedTextColor READ selectedTextColor WRITE setSelectedTextColor)
  Q_PROPERTY(bool selectable READ selectable WRITE setSelectable NOTIFY selectableChanged)
  Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectionChanged)
  /// \endcond
public:
  explicit MYQCPTextElement(QCustomPlot *parentPlot);
  MYQCPTextElement(QCustomPlot *parentPlot, const QString &text);
  MYQCPTextElement(QCustomPlot *parentPlot, const QString &text, double pointSize);
  MYQCPTextElement(QCustomPlot *parentPlot, const QString &text, const QString &fontFamily, double pointSize);
  MYQCPTextElement(QCustomPlot *parentPlot, const QString &text, const QFont &font);
  
  // getters:
  QString text() const { return mText; }
  int textFlags() const { return mTextFlags; }
  QFont font() const { return mFont; }
  QColor textColor() const { return mTextColor; }
  QFont selectedFont() const { return mSelectedFont; }
  QColor selectedTextColor() const { return mSelectedTextColor; }
  bool selectable() const { return mSelectable; }
  bool selected() const { return mSelected; }
  
  // setters:
  void setText(const QString &text);
  void setTextFlags(int flags);
  void setFont(const QFont &font);
  void setTextColor(const QColor &color);
  void setSelectedFont(const QFont &font);
  void setSelectedTextColor(const QColor &color);
  Q_SLOT void setSelectable(bool selectable);
  Q_SLOT void setSelected(bool selected);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  virtual void mousePressEvent(QMouseEvent *event, const QVariant &details) Q_DECL_OVERRIDE;
  virtual void mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos) Q_DECL_OVERRIDE;
  virtual void mouseDoubleClickEvent(QMouseEvent *event, const QVariant &details) Q_DECL_OVERRIDE;
  
signals:
  void selectionChanged(bool selected);
  void selectableChanged(bool selectable);
  void clicked(QMouseEvent *event);
  void doubleClicked(QMouseEvent *event);
  
protected:
  // property members:
  QString mText;
  int mTextFlags;
  QFont mFont;
  QColor mTextColor;
  QFont mSelectedFont;
  QColor mSelectedTextColor;
  QRect mTextBoundingRect;
  bool mSelectable, mSelected;
  
  // reimplemented virtual methods:
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const Q_DECL_OVERRIDE;
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual QSize minimumOuterSizeHint() const Q_DECL_OVERRIDE;
  virtual QSize maximumOuterSizeHint() const Q_DECL_OVERRIDE;
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged) Q_DECL_OVERRIDE;
  virtual void deselectEvent(bool *selectionStateChanged) Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  QFont mainFont() const;
  QColor mainTextColor() const;
  
private:
  Q_DISABLE_COPY(MYQCPTextElement)
};



/* end of 'src/layoutelements/layoutelement-textelement.h' */


/* including file 'src/layoutelements/layoutelement-colorscale.h', size 5923 */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */


class MYQCPColorScaleAxisRectPrivate : public MYQCPAxisRect
{
  Q_OBJECT
public:
  explicit MYQCPColorScaleAxisRectPrivate(MYQCPColorScale *parentColorScale);
protected:
  MYQCPColorScale *mParentColorScale;
  QImage mGradientImage;
  bool mGradientImageInvalidated;
  // re-using some methods of MYQCPAxisRect to make them available to friend class MYQCPColorScale
  using MYQCPAxisRect::calculateAutoMargin;
  using MYQCPAxisRect::mousePressEvent;
  using MYQCPAxisRect::mouseMoveEvent;
  using MYQCPAxisRect::mouseReleaseEvent;
  using MYQCPAxisRect::wheelEvent;
  using MYQCPAxisRect::update;
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  void updateGradientImage();
  Q_SLOT void axisSelectionChanged(MYQCPAxis::SelectableParts selectedParts);
  Q_SLOT void axisSelectableChanged(MYQCPAxis::SelectableParts selectableParts);
  friend class MYQCPColorScale;
};


class MYQCP_LIB_DECL MYQCPColorScale : public MYQCPLayoutElement
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(MYQCPAxis::AxisType type READ type WRITE setType)
  Q_PROPERTY(MYQCPRange dataRange READ dataRange WRITE setDataRange NOTIFY dataRangeChanged)
  Q_PROPERTY(MYQCPAxis::ScaleType dataScaleType READ dataScaleType WRITE setDataScaleType NOTIFY dataScaleTypeChanged)
  Q_PROPERTY(MYQCPColorGradient gradient READ gradient WRITE setGradient NOTIFY gradientChanged)
  Q_PROPERTY(QString label READ label WRITE setLabel)
  Q_PROPERTY(int barWidth READ barWidth WRITE setBarWidth)
  Q_PROPERTY(bool rangeDrag READ rangeDrag WRITE setRangeDrag)
  Q_PROPERTY(bool rangeZoom READ rangeZoom WRITE setRangeZoom)
  /// \endcond
public:
  explicit MYQCPColorScale(QCustomPlot *parentPlot);
  virtual ~MYQCPColorScale();
  
  // getters:
  MYQCPAxis *axis() const { return mColorAxis.data(); }
  MYQCPAxis::AxisType type() const { return mType; }
  MYQCPRange dataRange() const { return mDataRange; }
  MYQCPAxis::ScaleType dataScaleType() const { return mDataScaleType; }
  MYQCPColorGradient gradient() const { return mGradient; }
  QString label() const;
  int barWidth () const { return mBarWidth; }
  bool rangeDrag() const;
  bool rangeZoom() const;
  
  // setters:
  void setType(MYQCPAxis::AxisType type);
  Q_SLOT void setDataRange(const MYQCPRange &dataRange);
  Q_SLOT void setDataScaleType(MYQCPAxis::ScaleType scaleType);
  Q_SLOT void setGradient(const MYQCPColorGradient &gradient);
  void setLabel(const QString &str);
  void setBarWidth(int width);
  void setRangeDrag(bool enabled);
  void setRangeZoom(bool enabled);
  
  // non-property methods:
  QList<MYQCPColorMap*> colorMaps() const;
  void rescaleDataRange(bool onlyVisibleMaps);
  
  // reimplemented virtual methods:
  virtual void update(UpdatePhase phase) Q_DECL_OVERRIDE;
  
signals:
  void dataRangeChanged(const MYQCPRange &newRange);
  void dataScaleTypeChanged(MYQCPAxis::ScaleType scaleType);
  void gradientChanged(const MYQCPColorGradient &newGradient);

protected:
  // property members:
  MYQCPAxis::AxisType mType;
  MYQCPRange mDataRange;
  MYQCPAxis::ScaleType mDataScaleType;
  MYQCPColorGradient mGradient;
  int mBarWidth;
  
  // non-property members:
  QPointer<MYQCPColorScaleAxisRectPrivate> mAxisRect;
  QPointer<MYQCPAxis> mColorAxis;
  
  // reimplemented virtual methods:
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const Q_DECL_OVERRIDE;
  // events:
  virtual void mousePressEvent(QMouseEvent *event, const QVariant &details) Q_DECL_OVERRIDE;
  virtual void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos) Q_DECL_OVERRIDE;
  virtual void mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos) Q_DECL_OVERRIDE;
  virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
  
private:
  Q_DISABLE_COPY(MYQCPColorScale)
  
  friend class MYQCPColorScaleAxisRectPrivate;
};


/* end of 'src/layoutelements/layoutelement-colorscale.h' */


/* including file 'src/plottables/plottable-graph.h', size 9294              */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPGraphData
{
public:
  MYQCPGraphData();
  MYQCPGraphData(double key, double value);
  
  inline double sortKey() const { return key; }
  inline static MYQCPGraphData fromSortKey(double sortKey) { return MYQCPGraphData(sortKey, 0); }
  inline static bool sortKeyIsMainKey() { return true; }
  
  inline double mainKey() const { return key; }
  inline double mainValue() const { return value; }
  
  inline MYQCPRange valueRange() const { return MYQCPRange(value, value); }
  
  double key, value;
};
Q_DECLARE_TYPEINFO(MYQCPGraphData, Q_PRIMITIVE_TYPE);


/*! \typedef MYQCPGraphDataContainer
  
  Container for storing \ref MYQCPGraphData points. The data is stored sorted by \a key.
  
  This template instantiation is the container in which MYQCPGraph holds its data. For details about
  the generic container, see the documentation of the class template \ref MYQCPDataContainer.
  
  \see MYQCPGraphData, MYQCPGraph::setData
*/
typedef MYQCPDataContainer<MYQCPGraphData> MYQCPGraphDataContainer;

class MYQCP_LIB_DECL MYQCPGraph : public MYQCPAbstractPlottable1D<MYQCPGraphData>
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(LineStyle lineStyle READ lineStyle WRITE setLineStyle)
  Q_PROPERTY(MYQCPScatterStyle scatterStyle READ scatterStyle WRITE setScatterStyle)
  Q_PROPERTY(int scatterSkip READ scatterSkip WRITE setScatterSkip)
  Q_PROPERTY(MYQCPGraph* channelFillGraph READ channelFillGraph WRITE setChannelFillGraph)
  Q_PROPERTY(bool adaptiveSampling READ adaptiveSampling WRITE setAdaptiveSampling)
  /// \endcond
public:
  /*!
    Defines how the graph's line is represented visually in the plot. The line is drawn with the
    current pen of the graph (\ref setPen).
    \see setLineStyle
  */
  enum LineStyle { lsNone        ///< data points are not connected with any lines (e.g. data only represented
                                 ///< with symbols according to the scatter style, see \ref setScatterStyle)
                   ,lsLine       ///< data points are connected by a straight line
                   ,lsStepLeft   ///< line is drawn as steps where the step height is the value of the left data point
                   ,lsStepRight  ///< line is drawn as steps where the step height is the value of the right data point
                   ,lsStepCenter ///< line is drawn as steps where the step is in between two data points
                   ,lsImpulse    ///< each data point is represented by a line parallel to the value axis, which reaches from the data point to the zero-value-line
                 };
  Q_ENUMS(LineStyle)
  
  explicit MYQCPGraph(MYQCPAxis *keyAxis, MYQCPAxis *valueAxis);
  virtual ~MYQCPGraph();
  
  // getters:
  QSharedPointer<MYQCPGraphDataContainer> data() const { return mDataContainer; }
  LineStyle lineStyle() const { return mLineStyle; }
  MYQCPScatterStyle scatterStyle() const { return mScatterStyle; }
  int scatterSkip() const { return mScatterSkip; }
  MYQCPGraph *channelFillGraph() const { return mChannelFillGraph.data(); }
  bool adaptiveSampling() const { return mAdaptiveSampling; }
  
  // setters:
  void setData(QSharedPointer<MYQCPGraphDataContainer> data);
  void setData(const QVector<double> &keys, const QVector<double> &values, bool alreadySorted=false);
  void setLineStyle(LineStyle ls);
  void setScatterStyle(const MYQCPScatterStyle &style);
  void setScatterSkip(int skip);
  void setChannelFillGraph(MYQCPGraph *targetGraph);
  void setAdaptiveSampling(bool enabled);
  
  // non-property methods:
  void addData(const QVector<double> &keys, const QVector<double> &values, bool alreadySorted=false);
  void addData(double key, double value);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  virtual MYQCPRange getKeyRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth) const Q_DECL_OVERRIDE;
  virtual MYQCPRange getValueRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth, const MYQCPRange &inKeyRange=MYQCPRange()) const Q_DECL_OVERRIDE;
  
protected:
  // property members:
  LineStyle mLineStyle;
  MYQCPScatterStyle mScatterStyle;
  int mScatterSkip;
  QPointer<MYQCPGraph> mChannelFillGraph;
  bool mAdaptiveSampling;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual void drawLegendIcon(MYQCPPainter *painter, const QRectF &rect) const Q_DECL_OVERRIDE;
  
  // introduced virtual methods:
  virtual void drawFill(MYQCPPainter *painter, QVector<QPointF> *lines) const;
  virtual void drawScatterPlot(MYQCPPainter *painter, const QVector<QPointF> &scatters, const MYQCPScatterStyle &style) const;
  virtual void drawLinePlot(MYQCPPainter *painter, const QVector<QPointF> &lines) const;
  virtual void drawImpulsePlot(MYQCPPainter *painter, const QVector<QPointF> &lines) const;
  
  virtual void getOptimizedLineData(QVector<MYQCPGraphData> *lineData, const MYQCPGraphDataContainer::const_iterator &begin, const MYQCPGraphDataContainer::const_iterator &end) const;
  virtual void getOptimizedScatterData(QVector<MYQCPGraphData> *scatterData, MYQCPGraphDataContainer::const_iterator begin, MYQCPGraphDataContainer::const_iterator end) const;
  
  // non-virtual methods:
  void getVisibleDataBounds(MYQCPGraphDataContainer::const_iterator &begin, MYQCPGraphDataContainer::const_iterator &end, const MYQCPDataRange &rangeRestriction) const;
  void getLines(QVector<QPointF> *lines, const MYQCPDataRange &dataRange) const;
  void getScatters(QVector<QPointF> *scatters, const MYQCPDataRange &dataRange) const;
  QVector<QPointF> dataToLines(const QVector<MYQCPGraphData> &data) const;
  QVector<QPointF> dataToStepLeftLines(const QVector<MYQCPGraphData> &data) const;
  QVector<QPointF> dataToStepRightLines(const QVector<MYQCPGraphData> &data) const;
  QVector<QPointF> dataToStepCenterLines(const QVector<MYQCPGraphData> &data) const;
  QVector<QPointF> dataToImpulseLines(const QVector<MYQCPGraphData> &data) const;
  QVector<MYQCPDataRange> getNonNanSegments(const QVector<QPointF> *lineData, Qt::Orientation keyOrientation) const;
  QVector<QPair<MYQCPDataRange, MYQCPDataRange> > getOverlappingSegments(QVector<MYQCPDataRange> thisSegments, const QVector<QPointF> *thisData, QVector<MYQCPDataRange> otherSegments, const QVector<QPointF> *otherData) const;
  bool segmentsIntersect(double aLower, double aUpper, double bLower, double bUpper, int &bPrecedence) const;
  QPointF getFillBasePoint(QPointF matchingDataPoint) const;
  const QPolygonF getFillPolygon(const QVector<QPointF> *lineData, MYQCPDataRange segment) const;
  const QPolygonF getChannelFillPolygon(const QVector<QPointF> *lineData, MYQCPDataRange thisSegment, const QVector<QPointF> *otherData, MYQCPDataRange otherSegment) const;
  int findIndexBelowX(const QVector<QPointF> *data, double x) const;
  int findIndexAboveX(const QVector<QPointF> *data, double x) const;
  int findIndexBelowY(const QVector<QPointF> *data, double y) const;
  int findIndexAboveY(const QVector<QPointF> *data, double y) const;
  double pointDistance(const QPointF &pixelPoint, MYQCPGraphDataContainer::const_iterator &closestData) const;
  
  friend class QCustomPlot;
  friend class MYQCPLegend;
};
Q_DECLARE_METATYPE(MYQCPGraph::LineStyle)

/* end of 'src/plottables/plottable-graph.h' */


/* including file 'src/plottables/plottable-curve.h', size 7409              */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPCurveData
{
public:
  MYQCPCurveData();
  MYQCPCurveData(double t, double key, double value);
  
  inline double sortKey() const { return t; }
  inline static MYQCPCurveData fromSortKey(double sortKey) { return MYQCPCurveData(sortKey, 0, 0); }
  inline static bool sortKeyIsMainKey() { return false; }
  
  inline double mainKey() const { return key; }
  inline double mainValue() const { return value; }
  
  inline MYQCPRange valueRange() const { return MYQCPRange(value, value); }
  
  double t, key, value;
};
Q_DECLARE_TYPEINFO(MYQCPCurveData, Q_PRIMITIVE_TYPE);


/*! \typedef MYQCPCurveDataContainer
  
  Container for storing \ref MYQCPCurveData points. The data is stored sorted by \a t, so the \a
  sortKey() (returning \a t) is different from \a mainKey() (returning \a key).
  
  This template instantiation is the container in which MYQCPCurve holds its data. For details about
  the generic container, see the documentation of the class template \ref MYQCPDataContainer.
  
  \see MYQCPCurveData, MYQCPCurve::setData
*/
typedef MYQCPDataContainer<MYQCPCurveData> MYQCPCurveDataContainer;

class MYQCP_LIB_DECL MYQCPCurve : public MYQCPAbstractPlottable1D<MYQCPCurveData>
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(MYQCPScatterStyle scatterStyle READ scatterStyle WRITE setScatterStyle)
  Q_PROPERTY(int scatterSkip READ scatterSkip WRITE setScatterSkip)
  Q_PROPERTY(LineStyle lineStyle READ lineStyle WRITE setLineStyle)
  /// \endcond
public:
  /*!
    Defines how the curve's line is represented visually in the plot. The line is drawn with the
    current pen of the curve (\ref setPen).
    \see setLineStyle
  */
  enum LineStyle { lsNone  ///< No line is drawn between data points (e.g. only scatters)
                   ,lsLine ///< Data points are connected with a straight line
                 };
  Q_ENUMS(LineStyle)
  
  explicit MYQCPCurve(MYQCPAxis *keyAxis, MYQCPAxis *valueAxis);
  virtual ~MYQCPCurve();
  
  // getters:
  QSharedPointer<MYQCPCurveDataContainer> data() const { return mDataContainer; }
  MYQCPScatterStyle scatterStyle() const { return mScatterStyle; }
  int scatterSkip() const { return mScatterSkip; }
  LineStyle lineStyle() const { return mLineStyle; }
  
  // setters:
  void setData(QSharedPointer<MYQCPCurveDataContainer> data);
  void setData(const QVector<double> &t, const QVector<double> &keys, const QVector<double> &values, bool alreadySorted=false);
  void setData(const QVector<double> &keys, const QVector<double> &values);
  void setScatterStyle(const MYQCPScatterStyle &style);
  void setScatterSkip(int skip);
  void setLineStyle(LineStyle style);
  
  // non-property methods:
  void addData(const QVector<double> &t, const QVector<double> &keys, const QVector<double> &values, bool alreadySorted=false);
  void addData(const QVector<double> &keys, const QVector<double> &values);
  void addData(double t, double key, double value);
  void addData(double key, double value);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  virtual MYQCPRange getKeyRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth) const Q_DECL_OVERRIDE;
  virtual MYQCPRange getValueRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth, const MYQCPRange &inKeyRange=MYQCPRange()) const Q_DECL_OVERRIDE;
  
protected:
  // property members:
  MYQCPScatterStyle mScatterStyle;
  int mScatterSkip;
  LineStyle mLineStyle;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual void drawLegendIcon(MYQCPPainter *painter, const QRectF &rect) const Q_DECL_OVERRIDE;
  
  // introduced virtual methods:
  virtual void drawCurveLine(MYQCPPainter *painter, const QVector<QPointF> &lines) const;
  virtual void drawScatterPlot(MYQCPPainter *painter, const QVector<QPointF> &points, const MYQCPScatterStyle &style) const;
  
  // non-virtual methods:
  void getCurveLines(QVector<QPointF> *lines, const MYQCPDataRange &dataRange, double penWidth) const;
  void getScatters(QVector<QPointF> *scatters, const MYQCPDataRange &dataRange, double scatterWidth) const;
  int getRegion(double key, double value, double keyMin, double valueMax, double keyMax, double valueMin) const;
  QPointF getOptimizedPoint(int prevRegion, double prevKey, double prevValue, double key, double value, double keyMin, double valueMax, double keyMax, double valueMin) const;
  QVector<QPointF> getOptimizedCornerPoints(int prevRegion, int currentRegion, double prevKey, double prevValue, double key, double value, double keyMin, double valueMax, double keyMax, double valueMin) const;
  bool mayTraverse(int prevRegion, int currentRegion) const;
  bool getTraverse(double prevKey, double prevValue, double key, double value, double keyMin, double valueMax, double keyMax, double valueMin, QPointF &crossA, QPointF &crossB) const;
  void getTraverseCornerPoints(int prevRegion, int currentRegion, double keyMin, double valueMax, double keyMax, double valueMin, QVector<QPointF> &beforeTraverse, QVector<QPointF> &afterTraverse) const;
  double pointDistance(const QPointF &pixelPoint, MYQCPCurveDataContainer::const_iterator &closestData) const;
  
  friend class QCustomPlot;
  friend class MYQCPLegend;
};
Q_DECLARE_METATYPE(MYQCPCurve::LineStyle)

/* end of 'src/plottables/plottable-curve.h' */


/* including file 'src/plottables/plottable-bars.h', size 8933               */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPBarsGroup : public QObject
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(SpacingType spacingType READ spacingType WRITE setSpacingType)
  Q_PROPERTY(double spacing READ spacing WRITE setSpacing)
  /// \endcond
public:
  /*!
    Defines the ways the spacing between bars in the group can be specified. Thus it defines what
    the number passed to \ref setSpacing actually means.
    
    \see setSpacingType, setSpacing
  */
  enum SpacingType { stAbsolute       ///< Bar spacing is in absolute pixels
                     ,stAxisRectRatio ///< Bar spacing is given by a fraction of the axis rect size
                     ,stPlotCoords    ///< Bar spacing is in key coordinates and thus scales with the key axis range
                   };
  Q_ENUMS(SpacingType)
  
  explicit MYQCPBarsGroup(QCustomPlot *parentPlot);
  virtual ~MYQCPBarsGroup();
  
  // getters:
  SpacingType spacingType() const { return mSpacingType; }
  double spacing() const { return mSpacing; }
  
  // setters:
  void setSpacingType(SpacingType spacingType);
  void setSpacing(double spacing);
  
  // non-virtual methods:
  QList<MYQCPBars*> bars() const { return mBars; }
  MYQCPBars* bars(int index) const;
  int size() const { return mBars.size(); }
  bool isEmpty() const { return mBars.isEmpty(); }
  void clear();
  bool contains(MYQCPBars *bars) const { return mBars.contains(bars); }
  void append(MYQCPBars *bars);
  void insert(int i, MYQCPBars *bars);
  void remove(MYQCPBars *bars);
  
protected:
  // non-property members:
  QCustomPlot *mParentPlot;
  SpacingType mSpacingType;
  double mSpacing;
  QList<MYQCPBars*> mBars;
  
  // non-virtual methods:
  void registerBars(MYQCPBars *bars);
  void unregisterBars(MYQCPBars *bars);
  
  // virtual methods:
  double keyPixelOffset(const MYQCPBars *bars, double keyCoord);
  double getPixelSpacing(const MYQCPBars *bars, double keyCoord);
  
private:
  Q_DISABLE_COPY(MYQCPBarsGroup)
  
  friend class MYQCPBars;
};
Q_DECLARE_METATYPE(MYQCPBarsGroup::SpacingType)


class MYQCP_LIB_DECL MYQCPBarsData
{
public:
  MYQCPBarsData();
  MYQCPBarsData(double key, double value);
  
  inline double sortKey() const { return key; }
  inline static MYQCPBarsData fromSortKey(double sortKey) { return MYQCPBarsData(sortKey, 0); }
  inline static bool sortKeyIsMainKey() { return true; } 
  
  inline double mainKey() const { return key; }
  inline double mainValue() const { return value; }
  
  inline MYQCPRange valueRange() const { return MYQCPRange(value, value); } // note that bar base value isn't held in each MYQCPBarsData and thus can't/shouldn't be returned here
  
  double key, value;
};
Q_DECLARE_TYPEINFO(MYQCPBarsData, Q_PRIMITIVE_TYPE);


/*! \typedef MYQCPBarsDataContainer
  
  Container for storing \ref MYQCPBarsData points. The data is stored sorted by \a key.
  
  This template instantiation is the container in which MYQCPBars holds its data. For details about
  the generic container, see the documentation of the class template \ref MYQCPDataContainer.
  
  \see MYQCPBarsData, MYQCPBars::setData
*/
typedef MYQCPDataContainer<MYQCPBarsData> MYQCPBarsDataContainer;

class MYQCP_LIB_DECL MYQCPBars : public MYQCPAbstractPlottable1D<MYQCPBarsData>
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(double width READ width WRITE setWidth)
  Q_PROPERTY(WidthType widthType READ widthType WRITE setWidthType)
  Q_PROPERTY(MYQCPBarsGroup* barsGroup READ barsGroup WRITE setBarsGroup)
  Q_PROPERTY(double baseValue READ baseValue WRITE setBaseValue)
  Q_PROPERTY(double stackingGap READ stackingGap WRITE setStackingGap)
  Q_PROPERTY(MYQCPBars* barBelow READ barBelow)
  Q_PROPERTY(MYQCPBars* barAbove READ barAbove)
  /// \endcond
public:
  /*!
    Defines the ways the width of the bar can be specified. Thus it defines what the number passed
    to \ref setWidth actually means.
    
    \see setWidthType, setWidth
  */
  enum WidthType { wtAbsolute       ///< Bar width is in absolute pixels
                   ,wtAxisRectRatio ///< Bar width is given by a fraction of the axis rect size
                   ,wtPlotCoords    ///< Bar width is in key coordinates and thus scales with the key axis range
                 };
  Q_ENUMS(WidthType)
  
  explicit MYQCPBars(MYQCPAxis *keyAxis, MYQCPAxis *valueAxis);
  virtual ~MYQCPBars();
  
  // getters:
  double width() const { return mWidth; }
  WidthType widthType() const { return mWidthType; }
  MYQCPBarsGroup *barsGroup() const { return mBarsGroup; }
  double baseValue() const { return mBaseValue; }
  double stackingGap() const { return mStackingGap; }
  MYQCPBars *barBelow() const { return mBarBelow.data(); }
  MYQCPBars *barAbove() const { return mBarAbove.data(); }
  QSharedPointer<MYQCPBarsDataContainer> data() const { return mDataContainer; }
  
  // setters:
  void setData(QSharedPointer<MYQCPBarsDataContainer> data);
  void setData(const QVector<double> &keys, const QVector<double> &values, bool alreadySorted=false);
  void setWidth(double width);
  void setWidthType(WidthType widthType);
  void setBarsGroup(MYQCPBarsGroup *barsGroup);
  void setBaseValue(double baseValue);
  void setStackingGap(double pixels);
  
  // non-property methods:
  void addData(const QVector<double> &keys, const QVector<double> &values, bool alreadySorted=false);
  void addData(double key, double value);
  void moveBelow(MYQCPBars *bars);
  void moveAbove(MYQCPBars *bars);
  
  // reimplemented virtual methods:
  virtual MYQCPDataSelection selectTestRect(const QRectF &rect, bool onlySelectable) const Q_DECL_OVERRIDE;
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  virtual MYQCPRange getKeyRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth) const Q_DECL_OVERRIDE;
  virtual MYQCPRange getValueRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth, const MYQCPRange &inKeyRange=MYQCPRange()) const Q_DECL_OVERRIDE;
  virtual QPointF dataPixelPosition(int index) const Q_DECL_OVERRIDE;
  
protected:
  // property members:
  double mWidth;
  WidthType mWidthType;
  MYQCPBarsGroup *mBarsGroup;
  double mBaseValue;
  double mStackingGap;
  QPointer<MYQCPBars> mBarBelow, mBarAbove;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual void drawLegendIcon(MYQCPPainter *painter, const QRectF &rect) const Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  void getVisibleDataBounds(MYQCPBarsDataContainer::const_iterator &begin, MYQCPBarsDataContainer::const_iterator &end) const;
  QRectF getBarRect(double key, double value) const;
  void getPixelWidth(double key, double &lower, double &upper) const;
  double getStackedBaseValue(double key, bool positive) const;
  static void connectBars(MYQCPBars* lower, MYQCPBars* upper);
  
  friend class QCustomPlot;
  friend class MYQCPLegend;
  friend class MYQCPBarsGroup;
};
Q_DECLARE_METATYPE(MYQCPBars::WidthType)

/* end of 'src/plottables/plottable-bars.h' */


/* including file 'src/plottables/plottable-statisticalbox.h', size 7516     */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPStatisticalBoxData
{
public:
  MYQCPStatisticalBoxData();
  MYQCPStatisticalBoxData(double key, double minimum, double lowerQuartile, double median, double upperQuartile, double maximum, const QVector<double>& outliers=QVector<double>());
  
  inline double sortKey() const { return key; }
  inline static MYQCPStatisticalBoxData fromSortKey(double sortKey) { return MYQCPStatisticalBoxData(sortKey, 0, 0, 0, 0, 0); }
  inline static bool sortKeyIsMainKey() { return true; }
  
  inline double mainKey() const { return key; }
  inline double mainValue() const { return median; }
  
  inline MYQCPRange valueRange() const
  {
    MYQCPRange result(minimum, maximum);
    for (QVector<double>::const_iterator it = outliers.constBegin(); it != outliers.constEnd(); ++it)
      result.expand(*it);
    return result;
  }
  
  double key, minimum, lowerQuartile, median, upperQuartile, maximum;
  QVector<double> outliers;
};
Q_DECLARE_TYPEINFO(MYQCPStatisticalBoxData, Q_MOVABLE_TYPE);


/*! \typedef MYQCPStatisticalBoxDataContainer
  
  Container for storing \ref MYQCPStatisticalBoxData points. The data is stored sorted by \a key.
  
  This template instantiation is the container in which MYQCPStatisticalBox holds its data. For
  details about the generic container, see the documentation of the class template \ref
  MYQCPDataContainer.
  
  \see MYQCPStatisticalBoxData, MYQCPStatisticalBox::setData
*/
typedef MYQCPDataContainer<MYQCPStatisticalBoxData> MYQCPStatisticalBoxDataContainer;

class MYQCP_LIB_DECL MYQCPStatisticalBox : public MYQCPAbstractPlottable1D<MYQCPStatisticalBoxData>
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(double width READ width WRITE setWidth)
  Q_PROPERTY(double whiskerWidth READ whiskerWidth WRITE setWhiskerWidth)
  Q_PROPERTY(QPen whiskerPen READ whiskerPen WRITE setWhiskerPen)
  Q_PROPERTY(QPen whiskerBarPen READ whiskerBarPen WRITE setWhiskerBarPen)
  Q_PROPERTY(bool whiskerAntialiased READ whiskerAntialiased WRITE setWhiskerAntialiased)
  Q_PROPERTY(QPen medianPen READ medianPen WRITE setMedianPen)
  Q_PROPERTY(MYQCPScatterStyle outlierStyle READ outlierStyle WRITE setOutlierStyle)
  /// \endcond
public:
  explicit MYQCPStatisticalBox(MYQCPAxis *keyAxis, MYQCPAxis *valueAxis);
  
  // getters:
  QSharedPointer<MYQCPStatisticalBoxDataContainer> data() const { return mDataContainer; }
  double width() const { return mWidth; }
  double whiskerWidth() const { return mWhiskerWidth; }
  QPen whiskerPen() const { return mWhiskerPen; }
  QPen whiskerBarPen() const { return mWhiskerBarPen; }
  bool whiskerAntialiased() const { return mWhiskerAntialiased; }
  QPen medianPen() const { return mMedianPen; }
  MYQCPScatterStyle outlierStyle() const { return mOutlierStyle; }

  // setters:
  void setData(QSharedPointer<MYQCPStatisticalBoxDataContainer> data);
  void setData(const QVector<double> &keys, const QVector<double> &minimum, const QVector<double> &lowerQuartile, const QVector<double> &median, const QVector<double> &upperQuartile, const QVector<double> &maximum, bool alreadySorted=false);
  void setWidth(double width);
  void setWhiskerWidth(double width);
  void setWhiskerPen(const QPen &pen);
  void setWhiskerBarPen(const QPen &pen);
  void setWhiskerAntialiased(bool enabled);
  void setMedianPen(const QPen &pen);
  void setOutlierStyle(const MYQCPScatterStyle &style);
  
  // non-property methods:
  void addData(const QVector<double> &keys, const QVector<double> &minimum, const QVector<double> &lowerQuartile, const QVector<double> &median, const QVector<double> &upperQuartile, const QVector<double> &maximum, bool alreadySorted=false);
  void addData(double key, double minimum, double lowerQuartile, double median, double upperQuartile, double maximum, const QVector<double> &outliers=QVector<double>());
  
  // reimplemented virtual methods:
  virtual MYQCPDataSelection selectTestRect(const QRectF &rect, bool onlySelectable) const Q_DECL_OVERRIDE;
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  virtual MYQCPRange getKeyRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth) const Q_DECL_OVERRIDE;
  virtual MYQCPRange getValueRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth, const MYQCPRange &inKeyRange=MYQCPRange()) const Q_DECL_OVERRIDE;
  
protected:
  // property members:
  double mWidth;
  double mWhiskerWidth;
  QPen mWhiskerPen, mWhiskerBarPen;
  bool mWhiskerAntialiased;
  QPen mMedianPen;
  MYQCPScatterStyle mOutlierStyle;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual void drawLegendIcon(MYQCPPainter *painter, const QRectF &rect) const Q_DECL_OVERRIDE;
  
  // introduced virtual methods:
  virtual void drawStatisticalBox(MYQCPPainter *painter, MYQCPStatisticalBoxDataContainer::const_iterator it, const MYQCPScatterStyle &outlierStyle) const;
  
  // non-virtual methods:
  void getVisibleDataBounds(MYQCPStatisticalBoxDataContainer::const_iterator &begin, MYQCPStatisticalBoxDataContainer::const_iterator &end) const;
  QRectF getQuartileBox(MYQCPStatisticalBoxDataContainer::const_iterator it) const;
  QVector<QLineF> getWhiskerBackboneLines(MYQCPStatisticalBoxDataContainer::const_iterator it) const;
  QVector<QLineF> getWhiskerBarLines(MYQCPStatisticalBoxDataContainer::const_iterator it) const;
  
  friend class QCustomPlot;
  friend class MYQCPLegend;
};

/* end of 'src/plottables/plottable-statisticalbox.h' */


/* including file 'src/plottables/plottable-colormap.h', size 7070           */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPColorMapData
{
public:
  MYQCPColorMapData(int keySize, int valueSize, const MYQCPRange &keyRange, const MYQCPRange &valueRange);
  ~MYQCPColorMapData();
  MYQCPColorMapData(const MYQCPColorMapData &other);
  MYQCPColorMapData &operator=(const MYQCPColorMapData &other);
  
  // getters:
  int keySize() const { return mKeySize; }
  int valueSize() const { return mValueSize; }
  MYQCPRange keyRange() const { return mKeyRange; }
  MYQCPRange valueRange() const { return mValueRange; }
  MYQCPRange dataBounds() const { return mDataBounds; }
  double data(double key, double value);
  double cell(int keyIndex, int valueIndex);
  unsigned char alpha(int keyIndex, int valueIndex);
  
  // setters:
  void setSize(int keySize, int valueSize);
  void setKeySize(int keySize);
  void setValueSize(int valueSize);
  void setRange(const MYQCPRange &keyRange, const MYQCPRange &valueRange);
  void setKeyRange(const MYQCPRange &keyRange);
  void setValueRange(const MYQCPRange &valueRange);
  void setData(double key, double value, double z);
  void setCell(int keyIndex, int valueIndex, double z);
  void setAlpha(int keyIndex, int valueIndex, unsigned char alpha);
  
  // non-property methods:
  void recalculateDataBounds();
  void clear();
  void clearAlpha();
  void fill(double z);
  void fillAlpha(unsigned char alpha);
  bool isEmpty() const { return mIsEmpty; }
  void coordToCell(double key, double value, int *keyIndex, int *valueIndex) const;
  void cellToCoord(int keyIndex, int valueIndex, double *key, double *value) const;
  
protected:
  // property members:
  int mKeySize, mValueSize;
  MYQCPRange mKeyRange, mValueRange;
  bool mIsEmpty;
  
  // non-property members:
  double *mData;
  unsigned char *mAlpha;
  MYQCPRange mDataBounds;
  bool mDataModified;
  
  bool createAlpha(bool initializeOpaque=true);
  
  friend class MYQCPColorMap;
};


class MYQCP_LIB_DECL MYQCPColorMap : public MYQCPAbstractPlottable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(MYQCPRange dataRange READ dataRange WRITE setDataRange NOTIFY dataRangeChanged)
  Q_PROPERTY(MYQCPAxis::ScaleType dataScaleType READ dataScaleType WRITE setDataScaleType NOTIFY dataScaleTypeChanged)
  Q_PROPERTY(MYQCPColorGradient gradient READ gradient WRITE setGradient NOTIFY gradientChanged)
  Q_PROPERTY(bool interpolate READ interpolate WRITE setInterpolate)
  Q_PROPERTY(bool tightBoundary READ tightBoundary WRITE setTightBoundary)
  Q_PROPERTY(MYQCPColorScale* colorScale READ colorScale WRITE setColorScale)
  /// \endcond
public:
  explicit MYQCPColorMap(MYQCPAxis *keyAxis, MYQCPAxis *valueAxis);
  virtual ~MYQCPColorMap();
  
  // getters:
  MYQCPColorMapData *data() const { return mMapData; }
  MYQCPRange dataRange() const { return mDataRange; }
  MYQCPAxis::ScaleType dataScaleType() const { return mDataScaleType; }
  bool interpolate() const { return mInterpolate; }
  bool tightBoundary() const { return mTightBoundary; }
  MYQCPColorGradient gradient() const { return mGradient; }
  MYQCPColorScale *colorScale() const { return mColorScale.data(); }
  
  // setters:
  void setData(MYQCPColorMapData *data, bool copy=false);
  Q_SLOT void setDataRange(const MYQCPRange &dataRange);
  Q_SLOT void setDataScaleType(MYQCPAxis::ScaleType scaleType);
  Q_SLOT void setGradient(const MYQCPColorGradient &gradient);
  void setInterpolate(bool enabled);
  void setTightBoundary(bool enabled);
  void setColorScale(MYQCPColorScale *colorScale);
  
  // non-property methods:
  void rescaleDataRange(bool recalculateDataBounds=false);
  Q_SLOT void updateLegendIcon(Qt::TransformationMode transformMode=Qt::SmoothTransformation, const QSize &thumbSize=QSize(32, 18));
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  virtual MYQCPRange getKeyRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth) const Q_DECL_OVERRIDE;
  virtual MYQCPRange getValueRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth, const MYQCPRange &inKeyRange=MYQCPRange()) const Q_DECL_OVERRIDE;
  
signals:
  void dataRangeChanged(const MYQCPRange &newRange);
  void dataScaleTypeChanged(MYQCPAxis::ScaleType scaleType);
  void gradientChanged(const MYQCPColorGradient &newGradient);
  
protected:
  // property members:
  MYQCPRange mDataRange;
  MYQCPAxis::ScaleType mDataScaleType;
  MYQCPColorMapData *mMapData;
  MYQCPColorGradient mGradient;
  bool mInterpolate;
  bool mTightBoundary;
  QPointer<MYQCPColorScale> mColorScale;
  
  // non-property members:
  QImage mMapImage, mUndersampledMapImage;
  QPixmap mLegendIcon;
  bool mMapImageInvalidated;
  
  // introduced virtual methods:
  virtual void updateMapImage();
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual void drawLegendIcon(MYQCPPainter *painter, const QRectF &rect) const Q_DECL_OVERRIDE;
  
  friend class QCustomPlot;
  friend class MYQCPLegend;
};

/* end of 'src/plottables/plottable-colormap.h' */


/* including file 'src/plottables/plottable-financial.h', size 8622          */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPFinancialData
{
public:
  MYQCPFinancialData();
  MYQCPFinancialData(double key, double open, double high, double low, double close);
  
  inline double sortKey() const { return key; }
  inline static MYQCPFinancialData fromSortKey(double sortKey) { return MYQCPFinancialData(sortKey, 0, 0, 0, 0); }
  inline static bool sortKeyIsMainKey() { return true; } 
  
  inline double mainKey() const { return key; }
  inline double mainValue() const { return open; }
  
  inline MYQCPRange valueRange() const { return MYQCPRange(low, high); } // open and close must lie between low and high, so we don't need to check them
  
  double key, open, high, low, close;
};
Q_DECLARE_TYPEINFO(MYQCPFinancialData, Q_PRIMITIVE_TYPE);


/*! \typedef MYQCPFinancialDataContainer
  
  Container for storing \ref MYQCPFinancialData points. The data is stored sorted by \a key.
  
  This template instantiation is the container in which MYQCPFinancial holds its data. For details
  about the generic container, see the documentation of the class template \ref MYQCPDataContainer.
  
  \see MYQCPFinancialData, MYQCPFinancial::setData
*/
typedef MYQCPDataContainer<MYQCPFinancialData> MYQCPFinancialDataContainer;

class MYQCP_LIB_DECL MYQCPFinancial : public MYQCPAbstractPlottable1D<MYQCPFinancialData>
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(ChartStyle chartStyle READ chartStyle WRITE setChartStyle)
  Q_PROPERTY(double width READ width WRITE setWidth)
  Q_PROPERTY(WidthType widthType READ widthType WRITE setWidthType)
  Q_PROPERTY(bool twoColored READ twoColored WRITE setTwoColored)
  Q_PROPERTY(QBrush brushPositive READ brushPositive WRITE setBrushPositive)
  Q_PROPERTY(QBrush brushNegative READ brushNegative WRITE setBrushNegative)
  Q_PROPERTY(QPen penPositive READ penPositive WRITE setPenPositive)
  Q_PROPERTY(QPen penNegative READ penNegative WRITE setPenNegative)
  /// \endcond
public:
  /*!
    Defines the ways the width of the financial bar can be specified. Thus it defines what the
    number passed to \ref setWidth actually means.

    \see setWidthType, setWidth
  */
  enum WidthType { wtAbsolute       ///< width is in absolute pixels
                   ,wtAxisRectRatio ///< width is given by a fraction of the axis rect size
                   ,wtPlotCoords    ///< width is in key coordinates and thus scales with the key axis range
                 };
  Q_ENUMS(WidthType)
  
  /*!
    Defines the possible representations of OHLC data in the plot.
    
    \see setChartStyle
  */
  enum ChartStyle { csOhlc         ///< Open-High-Low-Close bar representation
                   ,csCandlestick  ///< Candlestick representation
                  };
  Q_ENUMS(ChartStyle)
  
  explicit MYQCPFinancial(MYQCPAxis *keyAxis, MYQCPAxis *valueAxis);
  virtual ~MYQCPFinancial();
  
  // getters:
  QSharedPointer<MYQCPFinancialDataContainer> data() const { return mDataContainer; }
  ChartStyle chartStyle() const { return mChartStyle; }
  double width() const { return mWidth; }
  WidthType widthType() const { return mWidthType; }
  bool twoColored() const { return mTwoColored; }
  QBrush brushPositive() const { return mBrushPositive; }
  QBrush brushNegative() const { return mBrushNegative; }
  QPen penPositive() const { return mPenPositive; }
  QPen penNegative() const { return mPenNegative; }
  
  // setters:
  void setData(QSharedPointer<MYQCPFinancialDataContainer> data);
  void setData(const QVector<double> &keys, const QVector<double> &open, const QVector<double> &high, const QVector<double> &low, const QVector<double> &close, bool alreadySorted=false);
  void setChartStyle(ChartStyle style);
  void setWidth(double width);
  void setWidthType(WidthType widthType);
  void setTwoColored(bool twoColored);
  void setBrushPositive(const QBrush &brush);
  void setBrushNegative(const QBrush &brush);
  void setPenPositive(const QPen &pen);
  void setPenNegative(const QPen &pen);
  
  // non-property methods:
  void addData(const QVector<double> &keys, const QVector<double> &open, const QVector<double> &high, const QVector<double> &low, const QVector<double> &close, bool alreadySorted=false);
  void addData(double key, double open, double high, double low, double close);
  
  // reimplemented virtual methods:
  virtual MYQCPDataSelection selectTestRect(const QRectF &rect, bool onlySelectable) const Q_DECL_OVERRIDE;
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  virtual MYQCPRange getKeyRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth) const Q_DECL_OVERRIDE;
  virtual MYQCPRange getValueRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth, const MYQCPRange &inKeyRange=MYQCPRange()) const Q_DECL_OVERRIDE;
  
  // static methods:
  static MYQCPFinancialDataContainer timeSeriesToOhlc(const QVector<double> &time, const QVector<double> &value, double timeBinSize, double timeBinOffset = 0);
  
protected:
  // property members:
  ChartStyle mChartStyle;
  double mWidth;
  WidthType mWidthType;
  bool mTwoColored;
  QBrush mBrushPositive, mBrushNegative;
  QPen mPenPositive, mPenNegative;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual void drawLegendIcon(MYQCPPainter *painter, const QRectF &rect) const Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  void drawOhlcPlot(MYQCPPainter *painter, const MYQCPFinancialDataContainer::const_iterator &begin, const MYQCPFinancialDataContainer::const_iterator &end, bool isSelected);
  void drawCandlestickPlot(MYQCPPainter *painter, const MYQCPFinancialDataContainer::const_iterator &begin, const MYQCPFinancialDataContainer::const_iterator &end, bool isSelected);
  double getPixelWidth(double key, double keyPixel) const;
  double ohlcSelectTest(const QPointF &pos, const MYQCPFinancialDataContainer::const_iterator &begin, const MYQCPFinancialDataContainer::const_iterator &end, MYQCPFinancialDataContainer::const_iterator &closestDataPoint) const;
  double candlestickSelectTest(const QPointF &pos, const MYQCPFinancialDataContainer::const_iterator &begin, const MYQCPFinancialDataContainer::const_iterator &end, MYQCPFinancialDataContainer::const_iterator &closestDataPoint) const;
  void getVisibleDataBounds(MYQCPFinancialDataContainer::const_iterator &begin, MYQCPFinancialDataContainer::const_iterator &end) const;
  QRectF selectionHitBox(MYQCPFinancialDataContainer::const_iterator it) const;
  
  friend class QCustomPlot;
  friend class MYQCPLegend;
};
Q_DECLARE_METATYPE(MYQCPFinancial::ChartStyle)

/* end of 'src/plottables/plottable-financial.h' */


/* including file 'src/plottables/plottable-errorbar.h', size 7727           */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPErrorBarsData
{
public:
  MYQCPErrorBarsData();
  explicit MYQCPErrorBarsData(double error);
  MYQCPErrorBarsData(double errorMinus, double errorPlus);
  
  double errorMinus, errorPlus;
};
Q_DECLARE_TYPEINFO(MYQCPErrorBarsData, Q_PRIMITIVE_TYPE);


/*! \typedef MYQCPErrorBarsDataContainer

  Container for storing \ref MYQCPErrorBarsData points. It is a typedef for <tt>QVector<\ref
  MYQCPErrorBarsData></tt>.

  This is the container in which \ref MYQCPErrorBars holds its data. Unlike most other data
  containers for plottables, it is not based on \ref MYQCPDataContainer. This is because the error
  bars plottable is special in that it doesn't store its own key and value coordinate per error
  bar. It adopts the key and value from the plottable to which the error bars shall be applied
  (\ref MYQCPErrorBars::setDataPlottable). So the stored \ref MYQCPErrorBarsData doesn't need a
  sortable key, but merely an index (as \c QVector provides), which maps one-to-one to the indices
  of the other plottable's data.

  \see MYQCPErrorBarsData, MYQCPErrorBars::setData
*/
typedef QVector<MYQCPErrorBarsData> MYQCPErrorBarsDataContainer;

class MYQCP_LIB_DECL MYQCPErrorBars : public MYQCPAbstractPlottable, public MYQCPPlottableInterface1D
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QSharedPointer<MYQCPErrorBarsDataContainer> data READ data WRITE setData)
  Q_PROPERTY(MYQCPAbstractPlottable* dataPlottable READ dataPlottable WRITE setDataPlottable)
  Q_PROPERTY(ErrorType errorType READ errorType WRITE setErrorType)
  Q_PROPERTY(double whiskerWidth READ whiskerWidth WRITE setWhiskerWidth)
  Q_PROPERTY(double symbolGap READ symbolGap WRITE setSymbolGap)
  /// \endcond
public:
  
  /*!
    Defines in which orientation the error bars shall appear. If your data needs both error
    dimensions, create two \ref MYQCPErrorBars with different \ref ErrorType.

    \see setErrorType
  */
  enum ErrorType { etKeyError    ///< The errors are for the key dimension (bars appear parallel to the key axis)
                   ,etValueError ///< The errors are for the value dimension (bars appear parallel to the value axis)
  };
  Q_ENUMS(ErrorType)
  
  explicit MYQCPErrorBars(MYQCPAxis *keyAxis, MYQCPAxis *valueAxis);
  virtual ~MYQCPErrorBars();
  // getters:
  QSharedPointer<MYQCPErrorBarsDataContainer> data() const { return mDataContainer; }
  MYQCPAbstractPlottable *dataPlottable() const { return mDataPlottable.data(); }
  ErrorType errorType() const { return mErrorType; }
  double whiskerWidth() const { return mWhiskerWidth; }
  double symbolGap() const { return mSymbolGap; }
  
  // setters:
  void setData(QSharedPointer<MYQCPErrorBarsDataContainer> data);
  void setData(const QVector<double> &error);
  void setData(const QVector<double> &errorMinus, const QVector<double> &errorPlus);
  void setDataPlottable(MYQCPAbstractPlottable* plottable);
  void setErrorType(ErrorType type);
  void setWhiskerWidth(double pixels);
  void setSymbolGap(double pixels);
  
  // non-property methods:
  void addData(const QVector<double> &error);
  void addData(const QVector<double> &errorMinus, const QVector<double> &errorPlus);
  void addData(double error);
  void addData(double errorMinus, double errorPlus);
  
  // virtual methods of 1d plottable interface:
  virtual int dataCount() const Q_DECL_OVERRIDE;
  virtual double dataMainKey(int index) const Q_DECL_OVERRIDE;
  virtual double dataSortKey(int index) const Q_DECL_OVERRIDE;
  virtual double dataMainValue(int index) const Q_DECL_OVERRIDE;
  virtual MYQCPRange dataValueRange(int index) const Q_DECL_OVERRIDE;
  virtual QPointF dataPixelPosition(int index) const Q_DECL_OVERRIDE;
  virtual bool sortKeyIsMainKey() const Q_DECL_OVERRIDE;
  virtual MYQCPDataSelection selectTestRect(const QRectF &rect, bool onlySelectable) const Q_DECL_OVERRIDE;
  virtual int findBegin(double sortKey, bool expandedRange=true) const Q_DECL_OVERRIDE;
  virtual int findEnd(double sortKey, bool expandedRange=true) const Q_DECL_OVERRIDE;
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  virtual MYQCPPlottableInterface1D *interface1D() Q_DECL_OVERRIDE { return this; }
  
protected:
  // property members:
  QSharedPointer<MYQCPErrorBarsDataContainer> mDataContainer;
  QPointer<MYQCPAbstractPlottable> mDataPlottable;
  ErrorType mErrorType;
  double mWhiskerWidth;
  double mSymbolGap;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual void drawLegendIcon(MYQCPPainter *painter, const QRectF &rect) const Q_DECL_OVERRIDE;
  virtual MYQCPRange getKeyRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth) const Q_DECL_OVERRIDE;
  virtual MYQCPRange getValueRange(bool &foundRange, MYQCP::SignDomain inSignDomain=MYQCP::sdBoth, const MYQCPRange &inKeyRange=MYQCPRange()) const Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  void getErrorBarLines(MYQCPErrorBarsDataContainer::const_iterator it, QVector<QLineF> &backbones, QVector<QLineF> &whiskers) const;
  void getVisibleDataBounds(MYQCPErrorBarsDataContainer::const_iterator &begin, MYQCPErrorBarsDataContainer::const_iterator &end, const MYQCPDataRange &rangeRestriction) const;
  double pointDistance(const QPointF &pixelPoint, MYQCPErrorBarsDataContainer::const_iterator &closestData) const;
  // helpers:
  void getDataSegments(QList<MYQCPDataRange> &selectedSegments, QList<MYQCPDataRange> &unselectedSegments) const;
  bool errorBarVisible(int index) const;
  bool rectIntersectsLine(const QRectF &pixelRect, const QLineF &line) const;
  
  friend class QCustomPlot;
  friend class MYQCPLegend;
};

/* end of 'src/plottables/plottable-errorbar.h' */


/* including file 'src/items/item-straightline.h', size 3117                 */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPItemStraightLine : public MYQCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  /// \endcond
public:
  explicit MYQCPItemStraightLine(QCustomPlot *parentPlot);
  virtual ~MYQCPItemStraightLine();
  
  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  
  MYQCPItemPosition * const point1;
  MYQCPItemPosition * const point2;
  
protected:
  // property members:
  QPen mPen, mSelectedPen;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  QLineF getRectClippedStraightLine(const MYQCPVector2D &point1, const MYQCPVector2D &vec, const QRect &rect) const;
  QPen mainPen() const;
};

/* end of 'src/items/item-straightline.h' */


/* including file 'src/items/item-line.h', size 3407                         */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPItemLine : public MYQCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  Q_PROPERTY(MYQCPLineEnding head READ head WRITE setHead)
  Q_PROPERTY(MYQCPLineEnding tail READ tail WRITE setTail)
  /// \endcond
public:
  explicit MYQCPItemLine(QCustomPlot *parentPlot);
  virtual ~MYQCPItemLine();
  
  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  MYQCPLineEnding head() const { return mHead; }
  MYQCPLineEnding tail() const { return mTail; }
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setHead(const MYQCPLineEnding &head);
  void setTail(const MYQCPLineEnding &tail);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  
  MYQCPItemPosition * const start;
  MYQCPItemPosition * const end;
  
protected:
  // property members:
  QPen mPen, mSelectedPen;
  MYQCPLineEnding mHead, mTail;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  QLineF getRectClippedLine(const MYQCPVector2D &start, const MYQCPVector2D &end, const QRect &rect) const;
  QPen mainPen() const;
};

/* end of 'src/items/item-line.h' */


/* including file 'src/items/item-curve.h', size 3379                        */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPItemCurve : public MYQCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  Q_PROPERTY(MYQCPLineEnding head READ head WRITE setHead)
  Q_PROPERTY(MYQCPLineEnding tail READ tail WRITE setTail)
  /// \endcond
public:
  explicit MYQCPItemCurve(QCustomPlot *parentPlot);
  virtual ~MYQCPItemCurve();
  
  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  MYQCPLineEnding head() const { return mHead; }
  MYQCPLineEnding tail() const { return mTail; }
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setHead(const MYQCPLineEnding &head);
  void setTail(const MYQCPLineEnding &tail);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  
  MYQCPItemPosition * const start;
  MYQCPItemPosition * const startDir;
  MYQCPItemPosition * const endDir;
  MYQCPItemPosition * const end;
  
protected:
  // property members:
  QPen mPen, mSelectedPen;
  MYQCPLineEnding mHead, mTail;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  QPen mainPen() const;
};

/* end of 'src/items/item-curve.h' */


/* including file 'src/items/item-rect.h', size 3688                         */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPItemRect : public MYQCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
  Q_PROPERTY(QBrush selectedBrush READ selectedBrush WRITE setSelectedBrush)
  /// \endcond
public:
  explicit MYQCPItemRect(QCustomPlot *parentPlot);
  virtual ~MYQCPItemRect();
  
  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  
  MYQCPItemPosition * const topLeft;
  MYQCPItemPosition * const bottomRight;
  MYQCPItemAnchor * const top;
  MYQCPItemAnchor * const topRight;
  MYQCPItemAnchor * const right;
  MYQCPItemAnchor * const bottom;
  MYQCPItemAnchor * const bottomLeft;
  MYQCPItemAnchor * const left;
  
protected:
  enum AnchorIndex {aiTop, aiTopRight, aiRight, aiBottom, aiBottomLeft, aiLeft};
  
  // property members:
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual QPointF anchorPixelPosition(int anchorId) const Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  QPen mainPen() const;
  QBrush mainBrush() const;
};

/* end of 'src/items/item-rect.h' */


/* including file 'src/items/item-text.h', size 5554                         */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPItemText : public MYQCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QColor color READ color WRITE setColor)
  Q_PROPERTY(QColor selectedColor READ selectedColor WRITE setSelectedColor)
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
  Q_PROPERTY(QBrush selectedBrush READ selectedBrush WRITE setSelectedBrush)
  Q_PROPERTY(QFont font READ font WRITE setFont)
  Q_PROPERTY(QFont selectedFont READ selectedFont WRITE setSelectedFont)
  Q_PROPERTY(QString text READ text WRITE setText)
  Q_PROPERTY(Qt::Alignment positionAlignment READ positionAlignment WRITE setPositionAlignment)
  Q_PROPERTY(Qt::Alignment textAlignment READ textAlignment WRITE setTextAlignment)
  Q_PROPERTY(double rotation READ rotation WRITE setRotation)
  Q_PROPERTY(QMargins padding READ padding WRITE setPadding)
  /// \endcond
public:
  explicit MYQCPItemText(QCustomPlot *parentPlot);
  virtual ~MYQCPItemText();
  
  // getters:
  QColor color() const { return mColor; }
  QColor selectedColor() const { return mSelectedColor; }
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  QFont font() const { return mFont; }
  QFont selectedFont() const { return mSelectedFont; }
  QString text() const { return mText; }
  Qt::Alignment positionAlignment() const { return mPositionAlignment; }
  Qt::Alignment textAlignment() const { return mTextAlignment; }
  double rotation() const { return mRotation; }
  QMargins padding() const { return mPadding; }
  
  // setters;
  void setColor(const QColor &color);
  void setSelectedColor(const QColor &color);
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  void setFont(const QFont &font);
  void setSelectedFont(const QFont &font);
  void setText(const QString &text);
  void setPositionAlignment(Qt::Alignment alignment);
  void setTextAlignment(Qt::Alignment alignment);
  void setRotation(double degrees);
  void setPadding(const QMargins &padding);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  
  MYQCPItemPosition * const position;
  MYQCPItemAnchor * const topLeft;
  MYQCPItemAnchor * const top;
  MYQCPItemAnchor * const topRight;
  MYQCPItemAnchor * const right;
  MYQCPItemAnchor * const bottomRight;
  MYQCPItemAnchor * const bottom;
  MYQCPItemAnchor * const bottomLeft;
  MYQCPItemAnchor * const left;
  
protected:
  enum AnchorIndex {aiTopLeft, aiTop, aiTopRight, aiRight, aiBottomRight, aiBottom, aiBottomLeft, aiLeft};
  
  // property members:
  QColor mColor, mSelectedColor;
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;
  QFont mFont, mSelectedFont;
  QString mText;
  Qt::Alignment mPositionAlignment;
  Qt::Alignment mTextAlignment;
  double mRotation;
  QMargins mPadding;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual QPointF anchorPixelPosition(int anchorId) const Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  QPointF getTextDrawPoint(const QPointF &pos, const QRectF &rect, Qt::Alignment positionAlignment) const;
  QFont mainFont() const;
  QColor mainColor() const;
  QPen mainPen() const;
  QBrush mainBrush() const;
};

/* end of 'src/items/item-text.h' */


/* including file 'src/items/item-ellipse.h', size 3868                      */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPItemEllipse : public MYQCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
  Q_PROPERTY(QBrush selectedBrush READ selectedBrush WRITE setSelectedBrush)
  /// \endcond
public:
  explicit MYQCPItemEllipse(QCustomPlot *parentPlot);
  virtual ~MYQCPItemEllipse();
  
  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  
  MYQCPItemPosition * const topLeft;
  MYQCPItemPosition * const bottomRight;
  MYQCPItemAnchor * const topLeftRim;
  MYQCPItemAnchor * const top;
  MYQCPItemAnchor * const topRightRim;
  MYQCPItemAnchor * const right;
  MYQCPItemAnchor * const bottomRightRim;
  MYQCPItemAnchor * const bottom;
  MYQCPItemAnchor * const bottomLeftRim;
  MYQCPItemAnchor * const left;
  MYQCPItemAnchor * const center;
  
protected:
  enum AnchorIndex {aiTopLeftRim, aiTop, aiTopRightRim, aiRight, aiBottomRightRim, aiBottom, aiBottomLeftRim, aiLeft, aiCenter};
  
  // property members:
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual QPointF anchorPixelPosition(int anchorId) const Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  QPen mainPen() const;
  QBrush mainBrush() const;
};

/* end of 'src/items/item-ellipse.h' */


/* including file 'src/items/item-pixmap.h', size 4373                       */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPItemPixmap : public MYQCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)
  Q_PROPERTY(bool scaled READ scaled WRITE setScaled)
  Q_PROPERTY(Qt::AspectRatioMode aspectRatioMode READ aspectRatioMode)
  Q_PROPERTY(Qt::TransformationMode transformationMode READ transformationMode)
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  /// \endcond
public:
  explicit MYQCPItemPixmap(QCustomPlot *parentPlot);
  virtual ~MYQCPItemPixmap();
  
  // getters:
  QPixmap pixmap() const { return mPixmap; }
  bool scaled() const { return mScaled; }
  Qt::AspectRatioMode aspectRatioMode() const { return mAspectRatioMode; }
  Qt::TransformationMode transformationMode() const { return mTransformationMode; }
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  
  // setters;
  void setPixmap(const QPixmap &pixmap);
  void setScaled(bool scaled, Qt::AspectRatioMode aspectRatioMode=Qt::KeepAspectRatio, Qt::TransformationMode transformationMode=Qt::SmoothTransformation);
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  
  MYQCPItemPosition * const topLeft;
  MYQCPItemPosition * const bottomRight;
  MYQCPItemAnchor * const top;
  MYQCPItemAnchor * const topRight;
  MYQCPItemAnchor * const right;
  MYQCPItemAnchor * const bottom;
  MYQCPItemAnchor * const bottomLeft;
  MYQCPItemAnchor * const left;
  
protected:
  enum AnchorIndex {aiTop, aiTopRight, aiRight, aiBottom, aiBottomLeft, aiLeft};
  
  // property members:
  QPixmap mPixmap;
  QPixmap mScaledPixmap;
  bool mScaled;
  bool mScaledPixmapInvalidated;
  Qt::AspectRatioMode mAspectRatioMode;
  Qt::TransformationMode mTransformationMode;
  QPen mPen, mSelectedPen;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual QPointF anchorPixelPosition(int anchorId) const Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  void updateScaledPixmap(QRect finalRect=QRect(), bool flipHorz=false, bool flipVert=false);
  QRect getFinalRect(bool *flippedHorz=0, bool *flippedVert=0) const;
  QPen mainPen() const;
};

/* end of 'src/items/item-pixmap.h' */


/* including file 'src/items/item-tracer.h', size 4762                       */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPItemTracer : public MYQCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
  Q_PROPERTY(QBrush selectedBrush READ selectedBrush WRITE setSelectedBrush)
  Q_PROPERTY(double size READ size WRITE setSize)
  Q_PROPERTY(TracerStyle style READ style WRITE setStyle)
  Q_PROPERTY(MYQCPGraph* graph READ graph WRITE setGraph)
  Q_PROPERTY(double graphKey READ graphKey WRITE setGraphKey)
  Q_PROPERTY(bool interpolating READ interpolating WRITE setInterpolating)
  /// \endcond
public:
  /*!
    The different visual appearances a tracer item can have. Some styles size may be controlled with \ref setSize.
    
    \see setStyle
  */
  enum TracerStyle { tsNone        ///< The tracer is not visible
                     ,tsPlus       ///< A plus shaped crosshair with limited size
                     ,tsCrosshair  ///< A plus shaped crosshair which spans the complete axis rect
                     ,tsCircle     ///< A circle
                     ,tsSquare     ///< A square
                   };
  Q_ENUMS(TracerStyle)

  explicit MYQCPItemTracer(QCustomPlot *parentPlot);
  virtual ~MYQCPItemTracer();

  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  double size() const { return mSize; }
  TracerStyle style() const { return mStyle; }
  MYQCPGraph *graph() const { return mGraph; }
  double graphKey() const { return mGraphKey; }
  bool interpolating() const { return mInterpolating; }

  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  void setSize(double size);
  void setStyle(TracerStyle style);
  void setGraph(MYQCPGraph *graph);
  void setGraphKey(double key);
  void setInterpolating(bool enabled);

  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  void updatePosition();

  MYQCPItemPosition * const position;

protected:
  // property members:
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;
  double mSize;
  TracerStyle mStyle;
  MYQCPGraph *mGraph;
  double mGraphKey;
  bool mInterpolating;

  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;

  // non-virtual methods:
  QPen mainPen() const;
  QBrush mainBrush() const;
};
Q_DECLARE_METATYPE(MYQCPItemTracer::TracerStyle)

/* end of 'src/items/item-tracer.h' */


/* including file 'src/items/item-bracket.h', size 3969                      */
/* commit ce344b3f96a62e5f652585e55f1ae7c7883cd45b 2018-06-25 01:03:39 +0200 */

class MYQCP_LIB_DECL MYQCPItemBracket : public MYQCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  Q_PROPERTY(double length READ length WRITE setLength)
  Q_PROPERTY(BracketStyle style READ style WRITE setStyle)
  /// \endcond
public:
  /*!
    Defines the various visual shapes of the bracket item. The appearance can be further modified
    by \ref setLength and \ref setPen.
    
    \see setStyle
  */
  enum BracketStyle { bsSquare  ///< A brace with angled edges
                      ,bsRound  ///< A brace with round edges
                      ,bsCurly  ///< A curly brace
                      ,bsCalligraphic ///< A curly brace with varying stroke width giving a calligraphic impression
  };
  Q_ENUMS(BracketStyle)

  explicit MYQCPItemBracket(QCustomPlot *parentPlot);
  virtual ~MYQCPItemBracket();
  
  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  double length() const { return mLength; }
  BracketStyle style() const { return mStyle; }
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setLength(double length);
  void setStyle(BracketStyle style);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;
  
  MYQCPItemPosition * const left;
  MYQCPItemPosition * const right;
  MYQCPItemAnchor * const center;
  
protected:
  // property members:
  enum AnchorIndex {aiCenter};
  QPen mPen, mSelectedPen;
  double mLength;
  BracketStyle mStyle;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter) Q_DECL_OVERRIDE;
  virtual QPointF anchorPixelPosition(int anchorId) const Q_DECL_OVERRIDE;
  
  // non-virtual methods:
  QPen mainPen() const;
};
Q_DECLARE_METATYPE(MYQCPItemBracket::BracketStyle)

/* end of 'src/items/item-bracket.h' */


#endif // QCUSTOMPLOT_H

