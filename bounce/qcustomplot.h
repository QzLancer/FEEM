/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011-2015 Emanuel Eichhammer                            **
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
**             Date: 22.12.15                                             **
**          Version: 1.3.2                                                **
****************************************************************************/

#ifndef QCUSTOMPLOT_H
#define QCUSTOMPLOT_H

#include <QObject>
#include <QPointer>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QVector>
#include <QString>
#include <QDateTime>
#include <QMultiMap>
#include <QFlags>
#include <QDebug>
#include <QVector2D>
#include <QStack>
#include <QCache>
#include <QMargins>
#include <qmath.h>
#include <limits>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#  include <qnumeric.h>
#  include <QPrinter>
#  include <QPrintEngine>
#else
#  include <QtNumeric>
#  include <QtPrintSupport/QtPrintSupport>
#endif

class MYQCPPainter;
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
class MYQCPItemPosition;
class MYQCPLayer;
class MYQCPPlotTitle;
class MYQCPLegend;
class MYQCPAbstractLegendItem;
class MYQCPColorMap;
class MYQCPColorScale;
class MYQCPBars;


/*! \file */


// decl definitions for shared library compilation/usage:
#if defined(QCUSTOMPLOT_COMPILE_LIBRARY)
#  define MYQCP_LIB_DECL Q_DECL_EXPORT
#elif defined(QCUSTOMPLOT_USE_LIBRARY)
#  define MYQCP_LIB_DECL Q_DECL_IMPORT
#else
#  define MYQCP_LIB_DECL
#endif

/*!
  The MYQCP Namespace contains general enums and QFlags used throughout the QCustomPlot library
*/
namespace MYQCP
{
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
                          ,aePlottables    = 0x0020 ///< <tt>0x0020</tt> Main lines of plottables (excluding error bars, see element \ref aeErrorBars)
                          ,aeItems         = 0x0040 ///< <tt>0x0040</tt> Main lines of items
                          ,aeScatters      = 0x0080 ///< <tt>0x0080</tt> Scatter symbols of plottables (excluding scatter symbols of type ssPixmap)
                          ,aeErrorBars     = 0x0100 ///< <tt>0x0100</tt> Error bars
                          ,aeFills         = 0x0200 ///< <tt>0x0200</tt> Borders of fills (e.g. under or between graphs)
                          ,aeZeroLine      = 0x0400 ///< <tt>0x0400</tt> Zero-lines, see \ref MYQCPGrid::setZeroLinePen
                          ,aeAll           = 0xFFFF ///< <tt>0xFFFF</tt> All elements
                          ,aeNone          = 0x0000 ///< <tt>0x0000</tt> No elements
                        };
Q_DECLARE_FLAGS(AntialiasedElements, AntialiasedElement)

/*!
  Defines plotting hints that control various aspects of the quality and speed of plotting.
  
  \see QCustomPlot::setPlottingHints
*/
enum PlottingHint { phNone            = 0x000 ///< <tt>0x000</tt> No hints are set
                    ,phFastPolylines  = 0x001 ///< <tt>0x001</tt> Graph/Curve lines are drawn with a faster method. This reduces the quality
                                              ///<                especially of the line segment joins. (Only relevant for solid line pens.)
                    ,phForceRepaint   = 0x002 ///< <tt>0x002</tt> causes an immediate repaint() instead of a soft update() when QCustomPlot::replot() is called with parameter \ref QCustomPlot::rpHint.
                                              ///<                This is set by default to prevent the plot from freezing on fast consecutive replots (e.g. user drags ranges with mouse).
                    ,phCacheLabels    = 0x004 ///< <tt>0x004</tt> axis (tick) labels will be cached as pixmaps, increasing replot performance.
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
                   ,iSelectOther      = 0x080 ///< <tt>0x080</tt> All other objects are selectable (e.g. your own derived layerables, the plot title,...)
                 };
Q_DECLARE_FLAGS(Interactions, Interaction)

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

} // end of namespace MYQCP

Q_DECLARE_OPERATORS_FOR_FLAGS(MYQCP::AntialiasedElements)
Q_DECLARE_OPERATORS_FOR_FLAGS(MYQCP::PlottingHints)
Q_DECLARE_OPERATORS_FOR_FLAGS(MYQCP::MarginSides)
Q_DECLARE_OPERATORS_FOR_FLAGS(MYQCP::Interactions)


class MYQCP_LIB_DECL MYQCPScatterStyle
{
  Q_GADGET
public:
  /*!
    Defines the shape used for scatter points.

    On plottables/items that draw scatters, the sizes of these visualizations (with exception of
    \ref ssDot and \ref ssPixmap) can be controlled with the \ref setSize function. Scatters are
    drawn with the pen and brush specified with \ref setPen and \ref setBrush.
  */
  Q_ENUMS(ScatterShape)
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
  void setSize(double size);
  void setShape(ScatterShape shape);
  void setPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setPixmap(const QPixmap &pixmap);
  void setCustomPath(const QPainterPath &customPath);

  // non-property methods:
  bool isNone() const { return mShape == ssNone; }
  bool isPenDefined() const { return mPenDefined; }
  void applyTo(MYQCPPainter *painter, const QPen &defaultPen) const;
  void drawShape(MYQCPPainter *painter, QPointF pos) const;
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
  Q_FLAGS(PainterMode PainterModes)
  Q_DECLARE_FLAGS(PainterModes, PainterMode)
  
  MYQCPPainter();
  MYQCPPainter(QPaintDevice *device);
  ~MYQCPPainter();
  
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


class MYQCP_LIB_DECL MYQCPLayer : public QObject
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QCustomPlot* parentPlot READ parentPlot)
  Q_PROPERTY(QString name READ name)
  Q_PROPERTY(int index READ index)
  Q_PROPERTY(QList<MYQCPLayerable*> children READ children)
  Q_PROPERTY(bool visible READ visible WRITE setVisible)
  /// \endcond
public:
  MYQCPLayer(QCustomPlot* parentPlot, const QString &layerName);
  ~MYQCPLayer();
  
  // getters:
  QCustomPlot *parentPlot() const { return mParentPlot; }
  QString name() const { return mName; }
  int index() const { return mIndex; }
  QList<MYQCPLayerable*> children() const { return mChildren; }
  bool visible() const { return mVisible; }
  
  // setters:
  void setVisible(bool visible);
  
protected:
  // property members:
  QCustomPlot *mParentPlot;
  QString mName;
  int mIndex;
  QList<MYQCPLayerable*> mChildren;
  bool mVisible;
  
  // non-virtual methods:
  void addChild(MYQCPLayerable *layerable, bool prepend);
  void removeChild(MYQCPLayerable *layerable);
  
private:
  Q_DISABLE_COPY(MYQCPLayer)
  
  friend class QCustomPlot;
  friend class MYQCPLayerable;
};

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
  ~MYQCPLayerable();
  
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
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
  virtual void deselectEvent(bool *selectionStateChanged);
  
  // non-property methods:
  void initializeParentPlot(QCustomPlot *parentPlot);
  void setParentLayerable(MYQCPLayerable* parentLayerable);
  bool moveToLayer(MYQCPLayer *layer, bool prepend);
  void applyAntialiasingHint(MYQCPPainter *painter, bool localAntialiased, MYQCP::AntialiasedElement overrideElement) const;
  
private:
  Q_DISABLE_COPY(MYQCPLayerable)
  
  friend class QCustomPlot;
  friend class MYQCPAxisRect;
};


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
  
  double size() const;
  double center() const;
  void normalize();
  void expand(const MYQCPRange &otherRange);
  MYQCPRange expanded(const MYQCPRange &otherRange) const;
  MYQCPRange sanitizedForLogScale() const;
  MYQCPRange sanitizedForLinScale() const;
  bool contains(double value) const;
  
  static bool validRange(double lower, double upper);
  static bool validRange(const MYQCPRange &range);
  static const double minRange; //1e-280;
  static const double maxRange; //1e280;
  
};
Q_DECLARE_TYPEINFO(MYQCPRange, Q_MOVABLE_TYPE);

/* documentation of inline functions */

/*! \fn MYQCPRange &MYQCPRange::operator+=(const double& value)
  
  Adds \a value to both boundaries of the range.
*/

/*! \fn MYQCPRange &MYQCPRange::operator-=(const double& value)
  
  Subtracts \a value from both boundaries of the range.
*/

/*! \fn MYQCPRange &MYQCPRange::operator*=(const double& value)
  
  Multiplies both boundaries of the range by \a value.
*/

/*! \fn MYQCPRange &MYQCPRange::operator/=(const double& value)
  
  Divides both boundaries of the range by \a value.
*/

/* end documentation of inline functions */

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


class MYQCP_LIB_DECL MYQCPMarginGroup : public QObject
{
  Q_OBJECT
public:
  MYQCPMarginGroup(QCustomPlot *parentPlot);
  ~MYQCPMarginGroup();
  
  // non-virtual methods:
  QList<MYQCPLayoutElement*> elements(MYQCP::MarginSide side) const { return mChildren.value(side); }
  bool isEmpty() const;
  void clear();
  
protected:
  // non-property members:
  QCustomPlot *mParentPlot;
  QHash<MYQCP::MarginSide, QList<MYQCPLayoutElement*> > mChildren;
  
  // non-virtual methods:
  int commonMargin(MYQCP::MarginSide side) const;
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
  void setMarginGroup(MYQCP::MarginSides sides, MYQCPMarginGroup *group);
  
  // introduced virtual methods:
  virtual void update(UpdatePhase phase);
  virtual QSize minimumSizeHint() const;
  virtual QSize maximumSizeHint() const;
  virtual QList<MYQCPLayoutElement*> elements(bool recursive) const;
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
protected:
  // property members:
  MYQCPLayout *mParentLayout;
  QSize mMinimumSize, mMaximumSize;
  QRect mRect, mOuterRect;
  QMargins mMargins, mMinimumMargins;
  MYQCP::MarginSides mAutoMargins;
  QHash<MYQCP::MarginSide, MYQCPMarginGroup*> mMarginGroups;
  
  // introduced virtual methods:
  virtual int calculateAutoMargin(MYQCP::MarginSide side);
  // events:
  virtual void mousePressEvent(QMouseEvent *event) {Q_UNUSED(event)}
  virtual void mouseMoveEvent(QMouseEvent *event) {Q_UNUSED(event)}
  virtual void mouseReleaseEvent(QMouseEvent *event) {Q_UNUSED(event)}
  virtual void mouseDoubleClickEvent(QMouseEvent *event) {Q_UNUSED(event)}
  virtual void wheelEvent(QWheelEvent *event) {Q_UNUSED(event)}
  
  // reimplemented virtual methods:
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const { Q_UNUSED(painter) }
  virtual void draw(MYQCPPainter *painter) { Q_UNUSED(painter) }
  virtual void parentPlotInitialized(QCustomPlot *parentPlot);

private:
  Q_DISABLE_COPY(MYQCPLayoutElement)
  
  friend class QCustomPlot;
  friend class MYQCPLayout;
  friend class MYQCPMarginGroup;
};


class MYQCP_LIB_DECL MYQCPLayout : public MYQCPLayoutElement
{
  Q_OBJECT
public:
  explicit MYQCPLayout();
  
  // reimplemented virtual methods:
  virtual void update(UpdatePhase phase);
  virtual QList<MYQCPLayoutElement*> elements(bool recursive) const;
  
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
  /// \endcond
public:
  explicit MYQCPLayoutGrid();
  virtual ~MYQCPLayoutGrid();
  
  // getters:
  int rowCount() const;
  int columnCount() const;
  QList<double> columnStretchFactors() const { return mColumnStretchFactors; }
  QList<double> rowStretchFactors() const { return mRowStretchFactors; }
  int columnSpacing() const { return mColumnSpacing; }
  int rowSpacing() const { return mRowSpacing; }
  
  // setters:
  void setColumnStretchFactor(int column, double factor);
  void setColumnStretchFactors(const QList<double> &factors);
  void setRowStretchFactor(int row, double factor);
  void setRowStretchFactors(const QList<double> &factors);
  void setColumnSpacing(int pixels);
  void setRowSpacing(int pixels);
  
  // reimplemented virtual methods:
  virtual void updateLayout();
  virtual int elementCount() const;
  virtual MYQCPLayoutElement* elementAt(int index) const;
  virtual MYQCPLayoutElement* takeAt(int index);
  virtual bool take(MYQCPLayoutElement* element);
  virtual QList<MYQCPLayoutElement*> elements(bool recursive) const;
  virtual void simplify();
  virtual QSize minimumSizeHint() const;
  virtual QSize maximumSizeHint() const;
  
  // non-virtual methods:
  MYQCPLayoutElement *element(int row, int column) const;
  bool addElement(int row, int column, MYQCPLayoutElement *element);
  bool hasElement(int row, int column);
  void expandTo(int newRowCount, int newColumnCount);
  void insertRow(int newIndex);
  void insertColumn(int newIndex);
  
protected:
  // property members:
  QList<QList<MYQCPLayoutElement*> > mElements;
  QList<double> mColumnStretchFactors;
  QList<double> mRowStretchFactors;
  int mColumnSpacing, mRowSpacing;
  
  // non-virtual methods:
  void getMinimumRowColSizes(QVector<int> *minColWidths, QVector<int> *minRowHeights) const;
  void getMaximumRowColSizes(QVector<int> *maxColWidths, QVector<int> *maxRowHeights) const;
  
private:
  Q_DISABLE_COPY(MYQCPLayoutGrid)
};


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
  virtual void updateLayout();
  virtual int elementCount() const;
  virtual MYQCPLayoutElement* elementAt(int index) const;
  virtual MYQCPLayoutElement* takeAt(int index);
  virtual bool take(MYQCPLayoutElement* element);
  virtual void simplify() {}
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
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
  Q_ENUMS(EndingStyle)
  enum EndingStyle { esNone          ///< No ending decoration
                     ,esFlatArrow    ///< A filled arrow head with a straight/flat back (a triangle)
                     ,esSpikeArrow   ///< A filled arrow head with an indented back
                     ,esLineArrow    ///< A non-filled arrow head with open back
                     ,esDisc         ///< A filled circle
                     ,esSquare       ///< A filled square
                     ,esDiamond      ///< A filled diamond (45° rotated square)
                     ,esBar          ///< A bar perpendicular to the line
                     ,esHalfBar      ///< A bar perpendicular to the line, pointing out to only one side (to which side can be changed with \ref setInverted)
                     ,esSkewedBar    ///< A bar that is skewed (skew controllable via \ref setLength)
                   };
  
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
  void draw(MYQCPPainter *painter, const QVector2D &pos, const QVector2D &dir) const;
  void draw(MYQCPPainter *painter, const QVector2D &pos, double angle) const;
  
protected:
  // property members:
  EndingStyle mStyle;
  double mWidth, mLength;
  bool mInverted;
};
Q_DECLARE_TYPEINFO(MYQCPLineEnding, Q_MOVABLE_TYPE);


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
  MYQCPGrid(MYQCPAxis *parentAxis);
  
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
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const;
  virtual void draw(MYQCPPainter *painter);
  
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
  Q_PROPERTY(double scaleLogBase READ scaleLogBase WRITE setScaleLogBase)
  Q_PROPERTY(MYQCPRange range READ range WRITE setRange NOTIFY rangeChanged)
  Q_PROPERTY(bool rangeReversed READ rangeReversed WRITE setRangeReversed)
  Q_PROPERTY(bool autoTicks READ autoTicks WRITE setAutoTicks)
  Q_PROPERTY(int autoTickCount READ autoTickCount WRITE setAutoTickCount)
  Q_PROPERTY(bool autoTickLabels READ autoTickLabels WRITE setAutoTickLabels)
  Q_PROPERTY(bool autoTickStep READ autoTickStep WRITE setAutoTickStep)
  Q_PROPERTY(bool autoSubTicks READ autoSubTicks WRITE setAutoSubTicks)
  Q_PROPERTY(bool ticks READ ticks WRITE setTicks)
  Q_PROPERTY(bool tickLabels READ tickLabels WRITE setTickLabels)
  Q_PROPERTY(int tickLabelPadding READ tickLabelPadding WRITE setTickLabelPadding)
  Q_PROPERTY(LabelType tickLabelType READ tickLabelType WRITE setTickLabelType)
  Q_PROPERTY(QFont tickLabelFont READ tickLabelFont WRITE setTickLabelFont)
  Q_PROPERTY(QColor tickLabelColor READ tickLabelColor WRITE setTickLabelColor)
  Q_PROPERTY(double tickLabelRotation READ tickLabelRotation WRITE setTickLabelRotation)
  Q_PROPERTY(LabelSide tickLabelSide READ tickLabelSide WRITE setTickLabelSide)
  Q_PROPERTY(QString dateTimeFormat READ dateTimeFormat WRITE setDateTimeFormat)
  Q_PROPERTY(Qt::TimeSpec dateTimeSpec READ dateTimeSpec WRITE setDateTimeSpec)
  Q_PROPERTY(QString numberFormat READ numberFormat WRITE setNumberFormat)
  Q_PROPERTY(int numberPrecision READ numberPrecision WRITE setNumberPrecision)
  Q_PROPERTY(double tickStep READ tickStep WRITE setTickStep)
  Q_PROPERTY(QVector<double> tickVector READ tickVector WRITE setTickVector)
  Q_PROPERTY(QVector<QString> tickVectorLabels READ tickVectorLabels WRITE setTickVectorLabels)
  Q_PROPERTY(int tickLengthIn READ tickLengthIn WRITE setTickLengthIn)
  Q_PROPERTY(int tickLengthOut READ tickLengthOut WRITE setTickLengthOut)
  Q_PROPERTY(int subTickCount READ subTickCount WRITE setSubTickCount)
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
  Q_FLAGS(AxisType AxisTypes)
  Q_DECLARE_FLAGS(AxisTypes, AxisType)
  /*!
    When automatic tick label generation is enabled (\ref setAutoTickLabels), defines how the
    coordinate of the tick is interpreted, i.e. translated into a string.
    
    \see setTickLabelType
  */
  enum LabelType { ltNumber    ///< Tick coordinate is regarded as normal number and will be displayed as such. (see \ref setNumberFormat)
                   ,ltDateTime ///< Tick coordinate is regarded as a date/time (seconds since 1970-01-01T00:00:00 UTC) and will be displayed and formatted as such. (for details, see \ref setDateTimeFormat)
                 };
  Q_ENUMS(LabelType)
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
                   ,stLogarithmic ///< Logarithmic scaling with correspondingly transformed plots and (major) tick marks at every base power (see \ref setScaleLogBase).
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
  Q_FLAGS(SelectablePart SelectableParts)
  Q_DECLARE_FLAGS(SelectableParts, SelectablePart)
  
  explicit MYQCPAxis(MYQCPAxisRect *parent, AxisType type);
  virtual ~MYQCPAxis();
  
  // getters:
  AxisType axisType() const { return mAxisType; }
  MYQCPAxisRect *axisRect() const { return mAxisRect; }
  ScaleType scaleType() const { return mScaleType; }
  double scaleLogBase() const { return mScaleLogBase; }
  const MYQCPRange range() const { return mRange; }
  bool rangeReversed() const { return mRangeReversed; }
  bool autoTicks() const { return mAutoTicks; }
  int autoTickCount() const { return mAutoTickCount; }
  bool autoTickLabels() const { return mAutoTickLabels; }
  bool autoTickStep() const { return mAutoTickStep; }
  bool autoSubTicks() const { return mAutoSubTicks; }
  bool ticks() const { return mTicks; }
  bool tickLabels() const { return mTickLabels; }
  int tickLabelPadding() const;
  LabelType tickLabelType() const { return mTickLabelType; }
  QFont tickLabelFont() const { return mTickLabelFont; }
  QColor tickLabelColor() const { return mTickLabelColor; }
  double tickLabelRotation() const;
  LabelSide tickLabelSide() const;
  QString dateTimeFormat() const { return mDateTimeFormat; }
  Qt::TimeSpec dateTimeSpec() const { return mDateTimeSpec; }
  QString numberFormat() const;
  int numberPrecision() const { return mNumberPrecision; }
  double tickStep() const { return mTickStep; }
  QVector<double> tickVector() const { return mTickVector; }
  QVector<QString> tickVectorLabels() const { return mTickVectorLabels; }
  int tickLengthIn() const;
  int tickLengthOut() const;
  int subTickCount() const { return mSubTickCount; }
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
  void setScaleLogBase(double base);
  Q_SLOT void setRange(const MYQCPRange &range);
  void setRange(double lower, double upper);
  void setRange(double position, double size, Qt::AlignmentFlag alignment);
  void setRangeLower(double lower);
  void setRangeUpper(double upper);
  void setRangeReversed(bool reversed);
  void setAutoTicks(bool on);
  void setAutoTickCount(int approximateCount);
  void setAutoTickLabels(bool on);
  void setAutoTickStep(bool on);
  void setAutoSubTicks(bool on);
  void setTicks(bool show);
  void setTickLabels(bool show);
  void setTickLabelPadding(int padding);
  void setTickLabelType(LabelType type);
  void setTickLabelFont(const QFont &font);
  void setTickLabelColor(const QColor &color);
  void setTickLabelRotation(double degrees);
  void setTickLabelSide(LabelSide side);
  void setDateTimeFormat(const QString &format);
  void setDateTimeSpec(const Qt::TimeSpec &timeSpec);
  void setNumberFormat(const QString &formatCode);
  void setNumberPrecision(int precision);
  void setTickStep(double step);
  void setTickVector(const QVector<double> &vec);
  void setTickVectorLabels(const QVector<QString> &vec);
  void setTickLength(int inside, int outside=0);
  void setTickLengthIn(int inside);
  void setTickLengthOut(int outside);
  void setSubTickCount(int count);
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
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
  // non-property methods:
  Qt::Orientation orientation() const { return mOrientation; }
  void moveRange(double diff);
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
  void ticksRequest();
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
  bool mTickLabels, mAutoTickLabels;
  //double mTickLabelRotation; // in MYQCPAxisPainter
  LabelType mTickLabelType;
  QFont mTickLabelFont, mSelectedTickLabelFont;
  QColor mTickLabelColor, mSelectedTickLabelColor;
  QString mDateTimeFormat;
  Qt::TimeSpec mDateTimeSpec;
  int mNumberPrecision;
  QLatin1Char mNumberFormatChar;
  bool mNumberBeautifulPowers;
  //bool mNumberMultiplyCross; // MYQCPAxisPainter
  // ticks and subticks:
  bool mTicks;
  double mTickStep;
  int mSubTickCount, mAutoTickCount;
  bool mAutoTicks, mAutoTickStep, mAutoSubTicks;
  //int mTickLengthIn, mTickLengthOut, mSubTickLengthIn, mSubTickLengthOut; // MYQCPAxisPainter
  QPen mTickPen, mSelectedTickPen;
  QPen mSubTickPen, mSelectedSubTickPen;
  // scale and range:
  MYQCPRange mRange;
  bool mRangeReversed;
  ScaleType mScaleType;
  double mScaleLogBase, mScaleLogBaseLogInv;
  
  // non-property members:
  MYQCPGrid *mGrid;
  MYQCPAxisPainterPrivate *mAxisPainter;
  int mLowestVisibleTick, mHighestVisibleTick;
  QVector<double> mTickVector;
  QVector<QString> mTickVectorLabels;
  QVector<double> mSubTickVector;
  bool mCachedMarginValid;
  int mCachedMargin;
  
  // introduced virtual methods:
  virtual void setupTickVectors();
  virtual void generateAutoTicks();
  virtual int calculateAutoSubTickCount(double tickStep) const;
  virtual int calculateMargin();
  
  // reimplemented virtual methods:
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const;
  virtual void draw(MYQCPPainter *painter);
  virtual MYQCP::Interaction selectionCategory() const;
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
  virtual void deselectEvent(bool *selectionStateChanged);
  
  // non-virtual methods:
  void visibleTickBounds(int &lowIndex, int &highIndex) const;
  double baseLog(double value) const;
  double basePow(double value) const;
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
    QString basePart, expPart;
    QRect baseBounds, expBounds, totalBounds, rotatedTotalBounds;
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


class MYQCP_LIB_DECL MYQCPAbstractPlottable : public MYQCPLayerable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QString name READ name WRITE setName)
  Q_PROPERTY(bool antialiasedFill READ antialiasedFill WRITE setAntialiasedFill)
  Q_PROPERTY(bool antialiasedScatters READ antialiasedScatters WRITE setAntialiasedScatters)
  Q_PROPERTY(bool antialiasedErrorBars READ antialiasedErrorBars WRITE setAntialiasedErrorBars)
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
  Q_PROPERTY(QBrush selectedBrush READ selectedBrush WRITE setSelectedBrush)
  Q_PROPERTY(MYQCPAxis* keyAxis READ keyAxis WRITE setKeyAxis)
  Q_PROPERTY(MYQCPAxis* valueAxis READ valueAxis WRITE setValueAxis)
  Q_PROPERTY(bool selectable READ selectable WRITE setSelectable NOTIFY selectableChanged)
  Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectionChanged)
  /// \endcond
public:
  MYQCPAbstractPlottable(MYQCPAxis *keyAxis, MYQCPAxis *valueAxis);
  
  // getters:
  QString name() const { return mName; }
  bool antialiasedFill() const { return mAntialiasedFill; }
  bool antialiasedScatters() const { return mAntialiasedScatters; }
  bool antialiasedErrorBars() const { return mAntialiasedErrorBars; }
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  QBrush brush() const { return mBrush; }
  QBrush selectedBrush() const { return mSelectedBrush; }
  MYQCPAxis *keyAxis() const { return mKeyAxis.data(); }
  MYQCPAxis *valueAxis() const { return mValueAxis.data(); }
  bool selectable() const { return mSelectable; }
  bool selected() const { return mSelected; }
  
  // setters:
  void setName(const QString &name);
  void setAntialiasedFill(bool enabled);
  void setAntialiasedScatters(bool enabled);
  void setAntialiasedErrorBars(bool enabled);
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  void setKeyAxis(MYQCPAxis *axis);
  void setValueAxis(MYQCPAxis *axis);
  Q_SLOT void setSelectable(bool selectable);
  Q_SLOT void setSelected(bool selected);

  // introduced virtual methods:
  virtual void clearData() = 0;
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const = 0;
  virtual bool addToLegend();
  virtual bool removeFromLegend() const;
  
  // non-property methods:
  void rescaleAxes(bool onlyEnlarge=false) const;
  void rescaleKeyAxis(bool onlyEnlarge=false) const;
  void rescaleValueAxis(bool onlyEnlarge=false) const;
  
signals:
  void selectionChanged(bool selected);
  void selectableChanged(bool selectable);
  
protected:
  /*!
    Represents negative and positive sign domain for passing to \ref getKeyRange and \ref getValueRange.
  */
  enum SignDomain { sdNegative  ///< The negative sign domain, i.e. numbers smaller than zero
                    ,sdBoth     ///< Both sign domains, including zero, i.e. all (rational) numbers
                    ,sdPositive ///< The positive sign domain, i.e. numbers greater than zero
                  };
  
  // property members:
  QString mName;
  bool mAntialiasedFill, mAntialiasedScatters, mAntialiasedErrorBars;
  QPen mPen, mSelectedPen;
  QBrush mBrush, mSelectedBrush;
  QPointer<MYQCPAxis> mKeyAxis, mValueAxis;
  bool mSelectable, mSelected;
  
  // reimplemented virtual methods:
  virtual QRect clipRect() const;
  virtual void draw(MYQCPPainter *painter) = 0;
  virtual MYQCP::Interaction selectionCategory() const;
  void applyDefaultAntialiasingHint(MYQCPPainter *painter) const;
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
  virtual void deselectEvent(bool *selectionStateChanged);
  
  // introduced virtual methods:
  virtual void drawLegendIcon(MYQCPPainter *painter, const QRectF &rect) const = 0;
  virtual MYQCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const = 0;
  virtual MYQCPRange getValueRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const = 0;
  
  // non-virtual methods:
  void coordsToPixels(double key, double value, double &x, double &y) const;
  const QPointF coordsToPixels(double key, double value) const;
  void pixelsToCoords(double x, double y, double &key, double &value) const;
  void pixelsToCoords(const QPointF &pixelPos, double &key, double &value) const;
  QPen mainPen() const;
  QBrush mainBrush() const;
  void applyFillAntialiasingHint(MYQCPPainter *painter) const;
  void applyScattersAntialiasingHint(MYQCPPainter *painter) const;
  void applyErrorBarsAntialiasingHint(MYQCPPainter *painter) const;
  double distSqrToLine(const QPointF &start, const QPointF &end, const QPointF &point) const;

private:
  Q_DISABLE_COPY(MYQCPAbstractPlottable)
  
  friend class QCustomPlot;
  friend class MYQCPAxis;
  friend class MYQCPPlottableLegendItem;
};


class MYQCP_LIB_DECL MYQCPItemAnchor
{
public:
  MYQCPItemAnchor(QCustomPlot *parentPlot, MYQCPAbstractItem *parentItem, const QString name, int anchorId=-1);
  virtual ~MYQCPItemAnchor();
  
  // getters:
  QString name() const { return mName; }
  virtual QPointF pixelPoint() const;
  
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
  
  MYQCPItemPosition(QCustomPlot *parentPlot, MYQCPAbstractItem *parentItem, const QString name);
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
  virtual QPointF pixelPoint() const;
  
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
  void setPixelPoint(const QPointF &pixelPoint);
  
protected:
  // property members:
  PositionType mPositionTypeX, mPositionTypeY;
  QPointer<MYQCPAxis> mKeyAxis, mValueAxis;
  QPointer<MYQCPAxisRect> mAxisRect;
  double mKey, mValue;
  MYQCPItemAnchor *mParentAnchorX, *mParentAnchorY;
  
  // reimplemented virtual methods:
  virtual MYQCPItemPosition *toMYQCPItemPosition() { return this; }
  
private:
  Q_DISABLE_COPY(MYQCPItemPosition)
  
};


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
  MYQCPAbstractItem(QCustomPlot *parentPlot);
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
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const = 0;
  
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
  virtual MYQCP::Interaction selectionCategory() const;
  virtual QRect clipRect() const;
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const;
  virtual void draw(MYQCPPainter *painter) = 0;
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
  virtual void deselectEvent(bool *selectionStateChanged);
  
  // introduced virtual methods:
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // non-virtual methods:
  double distSqrToLine(const QPointF &start, const QPointF &end, const QPointF &point) const;
  double rectSelectTest(const QRectF &rect, const QPointF &pos, bool filledRect) const;
  MYQCPItemPosition *createPosition(const QString &name);
  MYQCPItemAnchor *createAnchor(const QString &name, int anchorId);
  
private:
  Q_DISABLE_COPY(MYQCPAbstractItem)
  
  friend class QCustomPlot;
  friend class MYQCPItemAnchor;
};


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
  enum RefreshPriority { rpImmediate ///< The QCustomPlot surface is immediately refreshed, by calling QWidget::repaint() after the replot
                         ,rpQueued   ///< Queues the refresh such that it is performed at a slightly delayed point in time after the replot, by calling QWidget::update() after the replot
                         ,rpHint     ///< Whether to use immediate repaint or queued update depends on whether the plotting hint \ref MYQCP::phForceRepaint is set, see \ref setPlottingHints.
                       };
  
  explicit QCustomPlot(QWidget *parent = 0);
  virtual ~QCustomPlot();
  
  // getters:
  QRect viewport() const { return mViewport; }
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

  // setters:
  void setViewport(const QRect &rect);
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
  
  // non-property methods:
  // plottable interface:
  MYQCPAbstractPlottable *plottable(int index);
  MYQCPAbstractPlottable *plottable();
  bool addPlottable(MYQCPAbstractPlottable *plottable);
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
  bool addItem(MYQCPAbstractItem* item);
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
  Q_SLOT void rescaleAxes(bool onlyVisiblePlottables=false);
  
  QList<MYQCPAxis*> selectedAxes() const;
  QList<MYQCPLegend*> selectedLegends() const;
  Q_SLOT void deselectAll();
  
  bool savePdf(const QString &fileName, bool noCosmeticPen=false, int width=0, int height=0, const QString &pdfCreator=QString(), const QString &pdfTitle=QString());
  bool savePng(const QString &fileName, int width=0, int height=0, double scale=1.0, int quality=-1);
  bool saveJpg(const QString &fileName, int width=0, int height=0, double scale=1.0, int quality=-1);
  bool saveBmp(const QString &fileName, int width=0, int height=0, double scale=1.0);
  bool saveRastered(const QString &fileName, int width, int height, double scale, const char *format, int quality=-1);
  QPixmap toPixmap(int width=0, int height=0, double scale=1.0);
  void toPainter(MYQCPPainter *painter, int width=0, int height=0);
  Q_SLOT void replot(QCustomPlot::RefreshPriority refreshPriority=QCustomPlot::rpHint);
  
  MYQCPAxis *xAxis, *yAxis, *xAxis2, *yAxis2;
  MYQCPLegend *legend;
  
signals:
  void mouseDoubleClick(QMouseEvent *event);
  void mousePress(QMouseEvent *event);
  void mouseMove(QMouseEvent *event);
  void mouseRelease(QMouseEvent *event);
  void mouseWheel(QWheelEvent *event);
  
  void plottableClick(MYQCPAbstractPlottable *plottable, QMouseEvent *event);
  void plottableDoubleClick(MYQCPAbstractPlottable *plottable, QMouseEvent *event);
  void itemClick(MYQCPAbstractItem *item, QMouseEvent *event);
  void itemDoubleClick(MYQCPAbstractItem *item, QMouseEvent *event);
  void axisClick(MYQCPAxis *axis, MYQCPAxis::SelectablePart part, QMouseEvent *event);
  void axisDoubleClick(MYQCPAxis *axis, MYQCPAxis::SelectablePart part, QMouseEvent *event);
  void legendClick(MYQCPLegend *legend, MYQCPAbstractLegendItem *item, QMouseEvent *event);
  void legendDoubleClick(MYQCPLegend *legend,  MYQCPAbstractLegendItem *item, QMouseEvent *event);
  void titleClick(QMouseEvent *event, MYQCPPlotTitle *title);
  void titleDoubleClick(QMouseEvent *event, MYQCPPlotTitle *title);
  
  void selectionChangedByUser();
  void beforeReplot();
  void afterReplot();
  
protected:
  // property members:
  QRect mViewport;
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
  
  // non-property members:
  QPixmap mPaintBuffer;
  QPoint mMousePressPos;
  QPointer<MYQCPLayoutElement> mMouseEventElement;
  bool mReplotting;
  
  // reimplemented virtual methods:
  virtual QSize minimumSizeHint() const;
  virtual QSize sizeHint() const;
  virtual void paintEvent(QPaintEvent *event);
  virtual void resizeEvent(QResizeEvent *event);
  virtual void mouseDoubleClickEvent(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void wheelEvent(QWheelEvent *event);
  
  // introduced virtual methods:
  virtual void draw(MYQCPPainter *painter);
  virtual void axisRemoved(MYQCPAxis *axis);
  virtual void legendRemoved(MYQCPLegend *legend);
  
  // non-virtual methods:
  void updateLayerIndices() const;
  MYQCPLayerable *layerableAt(const QPointF &pos, bool onlySelectable, QVariant *selectionDetails=0) const;
  void drawBackground(MYQCPPainter *painter);
  
  friend class MYQCPLegend;
  friend class MYQCPAxis;
  friend class MYQCPLayer;
  friend class MYQCPAxisRect;
};


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
  
  MYQCPColorGradient(GradientPreset preset=gpCold);
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
  QRgb color(double position, const MYQCPRange &range, bool logarithmic=false);
  void loadPreset(GradientPreset preset);
  void clearColorStops();
  MYQCPColorGradient inverted() const;
  
protected:
  void updateColorBuffer();
  
  // property members:
  int mLevelCount;
  QMap<double, QColor> mColorStops;
  ColorInterpolation mColorInterpolation;
  bool mPeriodic;
  
  // non-property members:
  QVector<QRgb> mColorBuffer;
  bool mColorBufferInvalidated;
};


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
  bool backgroundScaled() const { return mBackgroundScaled; }
  Qt::AspectRatioMode backgroundScaledMode() const { return mBackgroundScaledMode; }
  Qt::Orientations rangeDrag() const { return mRangeDrag; }
  Qt::Orientations rangeZoom() const { return mRangeZoom; }
  MYQCPAxis *rangeDragAxis(Qt::Orientation orientation);
  MYQCPAxis *rangeZoomAxis(Qt::Orientation orientation);
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
  void setRangeZoomAxes(MYQCPAxis *horizontal, MYQCPAxis *vertical);
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
  virtual void update(UpdatePhase phase);
  virtual QList<MYQCPLayoutElement*> elements(bool recursive) const;

protected:
  // property members:
  QBrush mBackgroundBrush;
  QPixmap mBackgroundPixmap;
  QPixmap mScaledBackgroundPixmap;
  bool mBackgroundScaled;
  Qt::AspectRatioMode mBackgroundScaledMode;
  MYQCPLayoutInset *mInsetLayout;
  Qt::Orientations mRangeDrag, mRangeZoom;
  QPointer<MYQCPAxis> mRangeDragHorzAxis, mRangeDragVertAxis, mRangeZoomHorzAxis, mRangeZoomVertAxis;
  double mRangeZoomFactorHorz, mRangeZoomFactorVert;
  // non-property members:
  MYQCPRange mDragStartHorzRange, mDragStartVertRange;
  MYQCP::AntialiasedElements mAADragBackup, mNotAADragBackup;
  QPoint mDragStart;
  bool mDragging;
  QHash<MYQCPAxis::AxisType, QList<MYQCPAxis*> > mAxes;
  
  // reimplemented virtual methods:
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const;
  virtual void draw(MYQCPPainter *painter);
  virtual int calculateAutoMargin(MYQCP::MarginSide side);
  // events:
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void wheelEvent(QWheelEvent *event);
  
  // non-property methods:
  void drawBackground(MYQCPPainter *painter);
  void updateAxesOffset(MYQCPAxis::AxisType type);
  
private:
  Q_DISABLE_COPY(MYQCPAxisRect)
  
  friend class QCustomPlot;
};


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
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
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
  virtual MYQCP::Interaction selectionCategory() const;
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const;
  virtual QRect clipRect() const;
  virtual void draw(MYQCPPainter *painter) = 0;
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
  virtual void deselectEvent(bool *selectionStateChanged);
  
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
  virtual void draw(MYQCPPainter *painter);
  virtual QSize minimumSizeHint() const;
  
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
  enum SelectablePart { spNone       = 0x000  ///< <tt>0x000</tt> None
                        ,spLegendBox  = 0x001 ///< <tt>0x001</tt> The legend box (frame)
                        ,spItems      = 0x002 ///< <tt>0x002</tt> Legend items individually (see \ref selectedItems)
                      };
  Q_FLAGS(SelectablePart SelectableParts)
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
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
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
  virtual void parentPlotInitialized(QCustomPlot *parentPlot);
  virtual MYQCP::Interaction selectionCategory() const;
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const;
  virtual void draw(MYQCPPainter *painter);
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
  virtual void deselectEvent(bool *selectionStateChanged);
  
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


class MYQCP_LIB_DECL MYQCPPlotTitle : public MYQCPLayoutElement
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
  explicit MYQCPPlotTitle(QCustomPlot *parentPlot);
  explicit MYQCPPlotTitle(QCustomPlot *parentPlot, const QString &text);
  
  // getters:
  QString text() const { return mText; }
  QFont font() const { return mFont; }
  QColor textColor() const { return mTextColor; }
  QFont selectedFont() const { return mSelectedFont; }
  QColor selectedTextColor() const { return mSelectedTextColor; }
  bool selectable() const { return mSelectable; }
  bool selected() const { return mSelected; }
  
  // setters:
  void setText(const QString &text);
  void setFont(const QFont &font);
  void setTextColor(const QColor &color);
  void setSelectedFont(const QFont &font);
  void setSelectedTextColor(const QColor &color);
  Q_SLOT void setSelectable(bool selectable);
  Q_SLOT void setSelected(bool selected);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
signals:
  void selectionChanged(bool selected);
  void selectableChanged(bool selectable);
  
protected:
  // property members:
  QString mText;
  QFont mFont;
  QColor mTextColor;
  QFont mSelectedFont;
  QColor mSelectedTextColor;
  QRect mTextBoundingRect;
  bool mSelectable, mSelected;
  
  // reimplemented virtual methods:
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const;
  virtual void draw(MYQCPPainter *painter);
  virtual QSize minimumSizeHint() const;
  virtual QSize maximumSizeHint() const;
  // events:
  virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
  virtual void deselectEvent(bool *selectionStateChanged);
  
  // non-virtual methods:
  QFont mainFont() const;
  QColor mainTextColor() const;
  
private:
  Q_DISABLE_COPY(MYQCPPlotTitle)
};


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
  virtual void draw(MYQCPPainter *painter);
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
  virtual void update(UpdatePhase phase);
  
signals:
  void dataRangeChanged(MYQCPRange newRange);
  void dataScaleTypeChanged(MYQCPAxis::ScaleType scaleType);
  void gradientChanged(MYQCPColorGradient newGradient);

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
  virtual void applyDefaultAntialiasingHint(MYQCPPainter *painter) const;
  // events:
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void wheelEvent(QWheelEvent *event);
  
private:
  Q_DISABLE_COPY(MYQCPColorScale)
  
  friend class MYQCPColorScaleAxisRectPrivate;
};


/*! \file */



class MYQCP_LIB_DECL MYQCPData
{
public:
  MYQCPData();
  MYQCPData(double key, double value);
  double key, value;
  double keyErrorPlus, keyErrorMinus;
  double valueErrorPlus, valueErrorMinus;
};
Q_DECLARE_TYPEINFO(MYQCPData, Q_MOVABLE_TYPE);

/*! \typedef MYQCPDataMap
  Container for storing \ref MYQCPData items in a sorted fashion. The key of the map
  is the key member of the MYQCPData instance.
  
  This is the container in which MYQCPGraph holds its data.
  \see MYQCPData, MYQCPGraph::setData
*/
typedef QMap<double, MYQCPData> MYQCPDataMap;
typedef QMapIterator<double, MYQCPData> MYQCPDataMapIterator;
typedef QMutableMapIterator<double, MYQCPData> MYQCPDataMutableMapIterator;


class MYQCP_LIB_DECL MYQCPGraph : public MYQCPAbstractPlottable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(LineStyle lineStyle READ lineStyle WRITE setLineStyle)
  Q_PROPERTY(MYQCPScatterStyle scatterStyle READ scatterStyle WRITE setScatterStyle)
  Q_PROPERTY(ErrorType errorType READ errorType WRITE setErrorType)
  Q_PROPERTY(QPen errorPen READ errorPen WRITE setErrorPen)
  Q_PROPERTY(double errorBarSize READ errorBarSize WRITE setErrorBarSize)
  Q_PROPERTY(bool errorBarSkipSymbol READ errorBarSkipSymbol WRITE setErrorBarSkipSymbol)
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
  /*!
    Defines what kind of error bars are drawn for each data point
  */
  enum ErrorType { etNone   ///< No error bars are shown
                   ,etKey   ///< Error bars for the key dimension of the data point are shown
                   ,etValue ///< Error bars for the value dimension of the data point are shown
                   ,etBoth  ///< Error bars for both key and value dimensions of the data point are shown
                 };
  Q_ENUMS(ErrorType)
  
  explicit MYQCPGraph(MYQCPAxis *keyAxis, MYQCPAxis *valueAxis);
  virtual ~MYQCPGraph();
  
  // getters:
  MYQCPDataMap *data() const { return mData; }
  LineStyle lineStyle() const { return mLineStyle; }
  MYQCPScatterStyle scatterStyle() const { return mScatterStyle; }
  ErrorType errorType() const { return mErrorType; }
  QPen errorPen() const { return mErrorPen; }
  double errorBarSize() const { return mErrorBarSize; }
  bool errorBarSkipSymbol() const { return mErrorBarSkipSymbol; }
  MYQCPGraph *channelFillGraph() const { return mChannelFillGraph.data(); }
  bool adaptiveSampling() const { return mAdaptiveSampling; }
  
  // setters:
  void setData(MYQCPDataMap *data, bool copy=false);
  void setData(const QVector<double> &key, const QVector<double> &value);
  void setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError);
  void setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus);
  void setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueError);
  void setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus);
  void setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError, const QVector<double> &valueError);
  void setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus);
  void setLineStyle(LineStyle ls);
  void setScatterStyle(const MYQCPScatterStyle &style);
  void setErrorType(ErrorType errorType);
  void setErrorPen(const QPen &pen);
  void setErrorBarSize(double size);
  void setErrorBarSkipSymbol(bool enabled);
  void setChannelFillGraph(MYQCPGraph *targetGraph);
  void setAdaptiveSampling(bool enabled);
  
  // non-property methods:
  void addData(const MYQCPDataMap &dataMap);
  void addData(const MYQCPData &data);
  void addData(double key, double value);
  void addData(const QVector<double> &keys, const QVector<double> &values);
  void removeDataBefore(double key);
  void removeDataAfter(double key);
  void removeData(double fromKey, double toKey);
  void removeData(double key);
  
  // reimplemented virtual methods:
  virtual void clearData();
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  using MYQCPAbstractPlottable::rescaleAxes;
  using MYQCPAbstractPlottable::rescaleKeyAxis;
  using MYQCPAbstractPlottable::rescaleValueAxis;
  void rescaleAxes(bool onlyEnlarge, bool includeErrorBars) const; // overloads base class interface
  void rescaleKeyAxis(bool onlyEnlarge, bool includeErrorBars) const; // overloads base class interface
  void rescaleValueAxis(bool onlyEnlarge, bool includeErrorBars) const; // overloads base class interface
  
protected:
  // property members:
  MYQCPDataMap *mData;
  QPen mErrorPen;
  LineStyle mLineStyle;
  MYQCPScatterStyle mScatterStyle;
  ErrorType mErrorType;
  double mErrorBarSize;
  bool mErrorBarSkipSymbol;
  QPointer<MYQCPGraph> mChannelFillGraph;
  bool mAdaptiveSampling;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter);
  virtual void drawLegendIcon(MYQCPPainter *painter, const QRectF &rect) const;
  virtual MYQCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  virtual MYQCPRange getValueRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  virtual MYQCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain, bool includeErrors) const; // overloads base class interface
  virtual MYQCPRange getValueRange(bool &foundRange, SignDomain inSignDomain, bool includeErrors) const; // overloads base class interface
  
  // introduced virtual methods:
  virtual void drawFill(MYQCPPainter *painter, QVector<QPointF> *lineData) const;
  virtual void drawScatterPlot(MYQCPPainter *painter, QVector<MYQCPData> *scatterData) const;
  virtual void drawLinePlot(MYQCPPainter *painter, QVector<QPointF> *lineData) const;
  virtual void drawImpulsePlot(MYQCPPainter *painter, QVector<QPointF> *lineData) const;
  
  // non-virtual methods:
  void getPreparedData(QVector<MYQCPData> *lineData, QVector<MYQCPData> *scatterData) const;
  void getPlotData(QVector<QPointF> *lineData, QVector<MYQCPData> *scatterData) const;
  void getScatterPlotData(QVector<MYQCPData> *scatterData) const;
  void getLinePlotData(QVector<QPointF> *linePixelData, QVector<MYQCPData> *scatterData) const;
  void getStepLeftPlotData(QVector<QPointF> *linePixelData, QVector<MYQCPData> *scatterData) const;
  void getStepRightPlotData(QVector<QPointF> *linePixelData, QVector<MYQCPData> *scatterData) const;
  void getStepCenterPlotData(QVector<QPointF> *linePixelData, QVector<MYQCPData> *scatterData) const;
  void getImpulsePlotData(QVector<QPointF> *linePixelData, QVector<MYQCPData> *scatterData) const;
  void drawError(MYQCPPainter *painter, double x, double y, const MYQCPData &data) const;
  void getVisibleDataBounds(MYQCPDataMap::const_iterator &lower, MYQCPDataMap::const_iterator &upper) const;
  int countDataInBounds(const MYQCPDataMap::const_iterator &lower, const MYQCPDataMap::const_iterator &upper, int maxCount) const;
  void addFillBasePoints(QVector<QPointF> *lineData) const;
  void removeFillBasePoints(QVector<QPointF> *lineData) const;
  QPointF lowerFillBasePoint(double lowerKey) const;
  QPointF upperFillBasePoint(double upperKey) const;
  const QPolygonF getChannelFillPolygon(const QVector<QPointF> *lineData) const;
  int findIndexBelowX(const QVector<QPointF> *data, double x) const;
  int findIndexAboveX(const QVector<QPointF> *data, double x) const;
  int findIndexBelowY(const QVector<QPointF> *data, double y) const;
  int findIndexAboveY(const QVector<QPointF> *data, double y) const;
  double pointDistance(const QPointF &pixelPoint) const;
  
  friend class QCustomPlot;
  friend class MYQCPLegend;
};


/*! \file */



class MYQCP_LIB_DECL MYQCPCurveData
{
public:
  MYQCPCurveData();
  MYQCPCurveData(double t, double key, double value);
  double t, key, value;
};
Q_DECLARE_TYPEINFO(MYQCPCurveData, Q_MOVABLE_TYPE);

/*! \typedef MYQCPCurveDataMap
  Container for storing \ref MYQCPCurveData items in a sorted fashion. The key of the map
  is the t member of the MYQCPCurveData instance.
  
  This is the container in which MYQCPCurve holds its data.
  \see MYQCPCurveData, MYQCPCurve::setData
*/

typedef QMap<double, MYQCPCurveData> MYQCPCurveDataMap;
typedef QMapIterator<double, MYQCPCurveData> MYQCPCurveDataMapIterator;
typedef QMutableMapIterator<double, MYQCPCurveData> MYQCPCurveDataMutableMapIterator;


class MYQCP_LIB_DECL MYQCPCurve : public MYQCPAbstractPlottable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(MYQCPScatterStyle scatterStyle READ scatterStyle WRITE setScatterStyle)
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
  explicit MYQCPCurve(MYQCPAxis *keyAxis, MYQCPAxis *valueAxis);
  virtual ~MYQCPCurve();
  
  // getters:
  MYQCPCurveDataMap *data() const { return mData; }
  MYQCPScatterStyle scatterStyle() const { return mScatterStyle; }
  LineStyle lineStyle() const { return mLineStyle; }
  
  // setters:
  void setData(MYQCPCurveDataMap *data, bool copy=false);
  void setData(const QVector<double> &t, const QVector<double> &key, const QVector<double> &value);
  void setData(const QVector<double> &key, const QVector<double> &value);
  void setScatterStyle(const MYQCPScatterStyle &style);
  void setLineStyle(LineStyle style);
  
  // non-property methods:
  void addData(const MYQCPCurveDataMap &dataMap);
  void addData(const MYQCPCurveData &data);
  void addData(double t, double key, double value);
  void addData(double key, double value);
  void addData(const QVector<double> &ts, const QVector<double> &keys, const QVector<double> &values);
  void removeDataBefore(double t);
  void removeDataAfter(double t);
  void removeData(double fromt, double tot);
  void removeData(double t);
  
  // reimplemented virtual methods:
  virtual void clearData();
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
protected:
  // property members:
  MYQCPCurveDataMap *mData;
  MYQCPScatterStyle mScatterStyle;
  LineStyle mLineStyle;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter);
  virtual void drawLegendIcon(MYQCPPainter *painter, const QRectF &rect) const;
  virtual MYQCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  virtual MYQCPRange getValueRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  
  // introduced virtual methods:
  virtual void drawScatterPlot(MYQCPPainter *painter, const QVector<QPointF> *pointData) const;
  
  // non-virtual methods:
  void getCurveData(QVector<QPointF> *lineData) const;
  int getRegion(double x, double y, double rectLeft, double rectTop, double rectRight, double rectBottom) const;
  QPointF getOptimizedPoint(int prevRegion, double prevKey, double prevValue, double key, double value, double rectLeft, double rectTop, double rectRight, double rectBottom) const;
  QVector<QPointF> getOptimizedCornerPoints(int prevRegion, int currentRegion, double prevKey, double prevValue, double key, double value, double rectLeft, double rectTop, double rectRight, double rectBottom) const;
  bool mayTraverse(int prevRegion, int currentRegion) const;
  bool getTraverse(double prevKey, double prevValue, double key, double value, double rectLeft, double rectTop, double rectRight, double rectBottom, QPointF &crossA, QPointF &crossB) const;
  void getTraverseCornerPoints(int prevRegion, int currentRegion, double rectLeft, double rectTop, double rectRight, double rectBottom, QVector<QPointF> &beforeTraverse, QVector<QPointF> &afterTraverse) const;
  double pointDistance(const QPointF &pixelPoint) const;
  
  friend class QCustomPlot;
  friend class MYQCPLegend;
};


/*! \file */



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
  MYQCPBarsGroup(QCustomPlot *parentPlot);
  ~MYQCPBarsGroup();
  
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


class MYQCP_LIB_DECL MYQCPBarData
{
public:
  MYQCPBarData();
  MYQCPBarData(double key, double value);
  double key, value;
};
Q_DECLARE_TYPEINFO(MYQCPBarData, Q_MOVABLE_TYPE);

/*! \typedef MYQCPBarDataMap
  Container for storing \ref MYQCPBarData items in a sorted fashion. The key of the map
  is the key member of the MYQCPBarData instance.
  
  This is the container in which MYQCPBars holds its data.
  \see MYQCPBarData, MYQCPBars::setData
*/
typedef QMap<double, MYQCPBarData> MYQCPBarDataMap;
typedef QMapIterator<double, MYQCPBarData> MYQCPBarDataMapIterator;
typedef QMutableMapIterator<double, MYQCPBarData> MYQCPBarDataMutableMapIterator;


class MYQCP_LIB_DECL MYQCPBars : public MYQCPAbstractPlottable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(double width READ width WRITE setWidth)
  Q_PROPERTY(WidthType widthType READ widthType WRITE setWidthType)
  Q_PROPERTY(MYQCPBarsGroup* barsGroup READ barsGroup WRITE setBarsGroup)
  Q_PROPERTY(double baseValue READ baseValue WRITE setBaseValue)
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
  MYQCPBars *barBelow() const { return mBarBelow.data(); }
  MYQCPBars *barAbove() const { return mBarAbove.data(); }
  MYQCPBarDataMap *data() const { return mData; }
  
  // setters:
  void setWidth(double width);
  void setWidthType(WidthType widthType);
  void setBarsGroup(MYQCPBarsGroup *barsGroup);
  void setBaseValue(double baseValue);
  void setData(MYQCPBarDataMap *data, bool copy=false);
  void setData(const QVector<double> &key, const QVector<double> &value);
  
  // non-property methods:
  void moveBelow(MYQCPBars *bars);
  void moveAbove(MYQCPBars *bars);
  void addData(const MYQCPBarDataMap &dataMap);
  void addData(const MYQCPBarData &data);
  void addData(double key, double value);
  void addData(const QVector<double> &keys, const QVector<double> &values);
  void removeDataBefore(double key);
  void removeDataAfter(double key);
  void removeData(double fromKey, double toKey);
  void removeData(double key);
  
  // reimplemented virtual methods:
  virtual void clearData();
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
protected:
  // property members:
  MYQCPBarDataMap *mData;
  double mWidth;
  WidthType mWidthType;
  MYQCPBarsGroup *mBarsGroup;
  double mBaseValue;
  QPointer<MYQCPBars> mBarBelow, mBarAbove;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter);
  virtual void drawLegendIcon(MYQCPPainter *painter, const QRectF &rect) const;
  virtual MYQCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  virtual MYQCPRange getValueRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  
  // non-virtual methods:
  void getVisibleDataBounds(MYQCPBarDataMap::const_iterator &lower, MYQCPBarDataMap::const_iterator &upperEnd) const;
  QPolygonF getBarPolygon(double key, double value) const;
  void getPixelWidth(double key, double &lower, double &upper) const;
  double getStackedBaseValue(double key, bool positive) const;
  static void connectBars(MYQCPBars* lower, MYQCPBars* upper);
  
  friend class QCustomPlot;
  friend class MYQCPLegend;
  friend class MYQCPBarsGroup;
};


/*! \file */



class MYQCP_LIB_DECL MYQCPStatisticalBox : public MYQCPAbstractPlottable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(double key READ key WRITE setKey)
  Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
  Q_PROPERTY(double lowerQuartile READ lowerQuartile WRITE setLowerQuartile)
  Q_PROPERTY(double median READ median WRITE setMedian)
  Q_PROPERTY(double upperQuartile READ upperQuartile WRITE setUpperQuartile)
  Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
  Q_PROPERTY(QVector<double> outliers READ outliers WRITE setOutliers)
  Q_PROPERTY(double width READ width WRITE setWidth)
  Q_PROPERTY(double whiskerWidth READ whiskerWidth WRITE setWhiskerWidth)
  Q_PROPERTY(QPen whiskerPen READ whiskerPen WRITE setWhiskerPen)
  Q_PROPERTY(QPen whiskerBarPen READ whiskerBarPen WRITE setWhiskerBarPen)
  Q_PROPERTY(QPen medianPen READ medianPen WRITE setMedianPen)
  Q_PROPERTY(MYQCPScatterStyle outlierStyle READ outlierStyle WRITE setOutlierStyle)
  /// \endcond
public:
  explicit MYQCPStatisticalBox(MYQCPAxis *keyAxis, MYQCPAxis *valueAxis);
  
  // getters:
  double key() const { return mKey; }
  double minimum() const { return mMinimum; }
  double lowerQuartile() const { return mLowerQuartile; }
  double median() const { return mMedian; }
  double upperQuartile() const { return mUpperQuartile; }
  double maximum() const { return mMaximum; }
  QVector<double> outliers() const { return mOutliers; }
  double width() const { return mWidth; }
  double whiskerWidth() const { return mWhiskerWidth; }
  QPen whiskerPen() const { return mWhiskerPen; }
  QPen whiskerBarPen() const { return mWhiskerBarPen; }
  QPen medianPen() const { return mMedianPen; }
  MYQCPScatterStyle outlierStyle() const { return mOutlierStyle; }

  // setters:
  void setKey(double key);
  void setMinimum(double value);
  void setLowerQuartile(double value);
  void setMedian(double value);
  void setUpperQuartile(double value);
  void setMaximum(double value);
  void setOutliers(const QVector<double> &values);
  void setData(double key, double minimum, double lowerQuartile, double median, double upperQuartile, double maximum);
  void setWidth(double width);
  void setWhiskerWidth(double width);
  void setWhiskerPen(const QPen &pen);
  void setWhiskerBarPen(const QPen &pen);
  void setMedianPen(const QPen &pen);
  void setOutlierStyle(const MYQCPScatterStyle &style);
  
  // non-property methods:
  virtual void clearData();
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
protected:
  // property members:
  QVector<double> mOutliers;
  double mKey, mMinimum, mLowerQuartile, mMedian, mUpperQuartile, mMaximum;
  double mWidth;
  double mWhiskerWidth;
  QPen mWhiskerPen, mWhiskerBarPen, mMedianPen;
  MYQCPScatterStyle mOutlierStyle;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter);
  virtual void drawLegendIcon(MYQCPPainter *painter, const QRectF &rect) const;
  virtual MYQCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  virtual MYQCPRange getValueRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  
  // introduced virtual methods:
  virtual void drawQuartileBox(MYQCPPainter *painter, QRectF *quartileBox=0) const;
  virtual void drawMedian(MYQCPPainter *painter) const;
  virtual void drawWhiskers(MYQCPPainter *painter) const;
  virtual void drawOutliers(MYQCPPainter *painter) const;
  
  friend class QCustomPlot;
  friend class MYQCPLegend;
};


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
  
  // setters:
  void setSize(int keySize, int valueSize);
  void setKeySize(int keySize);
  void setValueSize(int valueSize);
  void setRange(const MYQCPRange &keyRange, const MYQCPRange &valueRange);
  void setKeyRange(const MYQCPRange &keyRange);
  void setValueRange(const MYQCPRange &valueRange);
  void setData(double key, double value, double z);
  void setCell(int keyIndex, int valueIndex, double z);
  
  // non-property methods:
  void recalculateDataBounds();
  void clear();
  void fill(double z);
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
  MYQCPRange mDataBounds;
  bool mDataModified;
  
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
  virtual void clearData();
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
signals:
  void dataRangeChanged(MYQCPRange newRange);
  void dataScaleTypeChanged(MYQCPAxis::ScaleType scaleType);
  void gradientChanged(MYQCPColorGradient newGradient);
  
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
  virtual void draw(MYQCPPainter *painter);
  virtual void drawLegendIcon(MYQCPPainter *painter, const QRectF &rect) const;
  virtual MYQCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  virtual MYQCPRange getValueRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  
  friend class QCustomPlot;
  friend class MYQCPLegend;
};


/*! \file */



class MYQCP_LIB_DECL MYQCPFinancialData
{
public:
  MYQCPFinancialData();
  MYQCPFinancialData(double key, double open, double high, double low, double close);
  double key, open, high, low, close;
};
Q_DECLARE_TYPEINFO(MYQCPFinancialData, Q_MOVABLE_TYPE);

/*! \typedef MYQCPFinancialDataMap
  Container for storing \ref MYQCPFinancialData items in a sorted fashion. The key of the map
  is the key member of the MYQCPFinancialData instance.
  
  This is the container in which MYQCPFinancial holds its data.
  \see MYQCPFinancial, MYQCPFinancial::setData
*/
typedef QMap<double, MYQCPFinancialData> MYQCPFinancialDataMap;
typedef QMapIterator<double, MYQCPFinancialData> MYQCPFinancialDataMapIterator;
typedef QMutableMapIterator<double, MYQCPFinancialData> MYQCPFinancialDataMutableMapIterator;


class MYQCP_LIB_DECL MYQCPFinancial : public MYQCPAbstractPlottable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(ChartStyle chartStyle READ chartStyle WRITE setChartStyle)
  Q_PROPERTY(double width READ width WRITE setWidth)
  Q_PROPERTY(bool twoColored READ twoColored WRITE setTwoColored)
  Q_PROPERTY(QBrush brushPositive READ brushPositive WRITE setBrushPositive)
  Q_PROPERTY(QBrush brushNegative READ brushNegative WRITE setBrushNegative)
  Q_PROPERTY(QPen penPositive READ penPositive WRITE setPenPositive)
  Q_PROPERTY(QPen penNegative READ penNegative WRITE setPenNegative)
  /// \endcond
public:
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
  MYQCPFinancialDataMap *data() const { return mData; }
  ChartStyle chartStyle() const { return mChartStyle; }
  double width() const { return mWidth; }
  bool twoColored() const { return mTwoColored; }
  QBrush brushPositive() const { return mBrushPositive; }
  QBrush brushNegative() const { return mBrushNegative; }
  QPen penPositive() const { return mPenPositive; }
  QPen penNegative() const { return mPenNegative; }
  
  
  // setters:
  void setData(MYQCPFinancialDataMap *data, bool copy=false);
  void setData(const QVector<double> &key, const QVector<double> &open, const QVector<double> &high, const QVector<double> &low, const QVector<double> &close);
  void setChartStyle(ChartStyle style);
  void setWidth(double width);
  void setTwoColored(bool twoColored);
  void setBrushPositive(const QBrush &brush);
  void setBrushNegative(const QBrush &brush);
  void setPenPositive(const QPen &pen);
  void setPenNegative(const QPen &pen);
  
  // non-property methods:
  void addData(const MYQCPFinancialDataMap &dataMap);
  void addData(const MYQCPFinancialData &data);
  void addData(double key, double open, double high, double low, double close);
  void addData(const QVector<double> &key, const QVector<double> &open, const QVector<double> &high, const QVector<double> &low, const QVector<double> &close);
  void removeDataBefore(double key);
  void removeDataAfter(double key);
  void removeData(double fromKey, double toKey);
  void removeData(double key);
  
  // reimplemented virtual methods:
  virtual void clearData();
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
  // static methods:
  static MYQCPFinancialDataMap timeSeriesToOhlc(const QVector<double> &time, const QVector<double> &value, double timeBinSize, double timeBinOffset = 0);
  
protected:
  // property members:
  MYQCPFinancialDataMap *mData;
  ChartStyle mChartStyle;
  double mWidth;
  bool mTwoColored;
  QBrush mBrushPositive, mBrushNegative;
  QPen mPenPositive, mPenNegative;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter);
  virtual void drawLegendIcon(MYQCPPainter *painter, const QRectF &rect) const;
  virtual MYQCPRange getKeyRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  virtual MYQCPRange getValueRange(bool &foundRange, SignDomain inSignDomain=sdBoth) const;
  
  // non-virtual methods:
  void drawOhlcPlot(MYQCPPainter *painter, const MYQCPFinancialDataMap::const_iterator &begin, const MYQCPFinancialDataMap::const_iterator &end);
  void drawCandlestickPlot(MYQCPPainter *painter, const MYQCPFinancialDataMap::const_iterator &begin, const MYQCPFinancialDataMap::const_iterator &end);
  double ohlcSelectTest(const QPointF &pos, const MYQCPFinancialDataMap::const_iterator &begin, const MYQCPFinancialDataMap::const_iterator &end) const;
  double candlestickSelectTest(const QPointF &pos, const MYQCPFinancialDataMap::const_iterator &begin, const MYQCPFinancialDataMap::const_iterator &end) const;
  void getVisibleDataBounds(MYQCPFinancialDataMap::const_iterator &lower, MYQCPFinancialDataMap::const_iterator &upper) const;
  
  friend class QCustomPlot;
  friend class MYQCPLegend;
};


class MYQCP_LIB_DECL MYQCPItemStraightLine : public MYQCPAbstractItem
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QPen pen READ pen WRITE setPen)
  Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
  /// \endcond
public:
  MYQCPItemStraightLine(QCustomPlot *parentPlot);
  virtual ~MYQCPItemStraightLine();
  
  // getters:
  QPen pen() const { return mPen; }
  QPen selectedPen() const { return mSelectedPen; }
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  
  // reimplemented virtual methods:
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
  MYQCPItemPosition * const point1;
  MYQCPItemPosition * const point2;
  
protected:
  // property members:
  QPen mPen, mSelectedPen;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter);
  
  // non-virtual methods:
  double distToStraightLine(const QVector2D &point1, const QVector2D &vec, const QVector2D &point) const;
  QLineF getRectClippedStraightLine(const QVector2D &point1, const QVector2D &vec, const QRect &rect) const;
  QPen mainPen() const;
};


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
  MYQCPItemLine(QCustomPlot *parentPlot);
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
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
  MYQCPItemPosition * const start;
  MYQCPItemPosition * const end;
  
protected:
  // property members:
  QPen mPen, mSelectedPen;
  MYQCPLineEnding mHead, mTail;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter);
  
  // non-virtual methods:
  QLineF getRectClippedLine(const QVector2D &start, const QVector2D &end, const QRect &rect) const;
  QPen mainPen() const;
};


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
  MYQCPItemCurve(QCustomPlot *parentPlot);
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
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
  MYQCPItemPosition * const start;
  MYQCPItemPosition * const startDir;
  MYQCPItemPosition * const endDir;
  MYQCPItemPosition * const end;
  
protected:
  // property members:
  QPen mPen, mSelectedPen;
  MYQCPLineEnding mHead, mTail;
  
  // reimplemented virtual methods:
  virtual void draw(MYQCPPainter *painter);
  
  // non-virtual methods:
  QPen mainPen() const;
};


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
  MYQCPItemRect(QCustomPlot *parentPlot);
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
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
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
  virtual void draw(MYQCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // non-virtual methods:
  QPen mainPen() const;
  QBrush mainBrush() const;
};


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
  MYQCPItemText(QCustomPlot *parentPlot);
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
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
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
  virtual void draw(MYQCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // non-virtual methods:
  QPointF getTextDrawPoint(const QPointF &pos, const QRectF &rect, Qt::Alignment positionAlignment) const;
  QFont mainFont() const;
  QColor mainColor() const;
  QPen mainPen() const;
  QBrush mainBrush() const;
};


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
  MYQCPItemEllipse(QCustomPlot *parentPlot);
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
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
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
  virtual void draw(MYQCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // non-virtual methods:
  QPen mainPen() const;
  QBrush mainBrush() const;
};


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
  MYQCPItemPixmap(QCustomPlot *parentPlot);
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
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
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
  virtual void draw(MYQCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // non-virtual methods:
  void updateScaledPixmap(QRect finalRect=QRect(), bool flipHorz=false, bool flipVert=false);
  QRect getFinalRect(bool *flippedHorz=0, bool *flippedVert=0) const;
  QPen mainPen() const;
};


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

  MYQCPItemTracer(QCustomPlot *parentPlot);
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
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
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
  virtual void draw(MYQCPPainter *painter);

  // non-virtual methods:
  QPen mainPen() const;
  QBrush mainBrush() const;
};


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
  enum BracketStyle { bsSquare  ///< A brace with angled edges
                      ,bsRound  ///< A brace with round edges
                      ,bsCurly  ///< A curly brace
                      ,bsCalligraphic ///< A curly brace with varying stroke width giving a calligraphic impression
  };

  MYQCPItemBracket(QCustomPlot *parentPlot);
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
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
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
  virtual void draw(MYQCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // non-virtual methods:
  QPen mainPen() const;
};

#endif // QCUSTOMPLOT_H

