#ifndef SEGMENTCUSTOMRECTITEM_HPP
#define SEGMENTCUSTOMRECTITEM_HPP

#include <score/document/DocumentContext.hpp>

#include <QGraphicsRectItem>

#include <verdigris>

#include <vector>

namespace SEGMent
{
static constexpr qreal titleBarHeight = 30;
static constexpr qreal borderWidth = 5;

class ZoomView;
class Window;
//! Base class for visual items in the canvas
class RectItem : public QObject, public QGraphicsRectItem
{
  W_OBJECT(RectItem)
public:
  const score::DocumentContext& context;

  explicit RectItem(
      qreal x,
      qreal y,
      qreal w,
      qreal h,
      bool constrainPosToParent,
      const score::DocumentContext& ctx,
      ZoomView& view,
      QGraphicsItem* parent = Q_NULLPTR);
  void setMinSize(qreal minWidth, qreal minHeight);
  void setRect(const QRectF& rect);
  void setRect(qreal x, qreal y, qreal w, qreal h);
  void moveBy(qreal dx, qreal dy);

  void on_sizeChanged() W_SIGNAL(on_sizeChanged);
  virtual void updateRect();

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;

  ZoomView& m_view;
  std::vector<Window*> m_childWindows;

  qreal m_minWidth = 0;
  qreal m_minHeight = 0;

  bool m_posConstrainedToParent = false;

  // QGraphicsItem interface
public:
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

constexpr inline QPointF expected_pos(const QPointF& pos, const QRectF& prect)
{
  return {pos.x() * prect.width(), pos.y() * prect.height()};
}

constexpr inline QSizeF expected_size(const QSizeF& sz, const QRectF& prect)
{
  return {sz.width() * prect.width(), sz.height() * prect.width()};
}

constexpr inline QRectF expected_rect(const QSizeF& sz, const QRectF& prect)
{
  return {0., 0., sz.width() * prect.width(), sz.height() * prect.width()};
}
constexpr inline qreal expected_height(const QSizeF& sz, const QRectF& prect)
{
  return sz.height() * prect.width();
}
} // namespace SEGMent

#endif // SEGMENTCUSTOMRECTITEM_HPP
