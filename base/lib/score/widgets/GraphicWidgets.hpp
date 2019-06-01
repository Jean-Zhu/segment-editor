#pragma once
#include <score/widgets/SignalUtils.hpp>

#include <ossia/detail/math.hpp>

#include <QDoubleSpinBox>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneEvent>
#include <QKeyEvent>
#include <QPainter>

#include <score_lib_base_export.h>
#include <wobjectdefs.h>

namespace score
{
class SCORE_LIB_BASE_EXPORT QGraphicsPixmapButton final
    : public QObject,
      public QGraphicsPixmapItem
{
  W_OBJECT(QGraphicsPixmapButton)
  const QPixmap m_pressed, m_released;

public:
  QGraphicsPixmapButton(
      QPixmap pressed,
      QPixmap released,
      QGraphicsItem* parent);

public:
  void clicked() W_SIGNAL(clicked);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};

class SCORE_LIB_BASE_EXPORT QGraphicsPixmapToggle final
    : public QObject,
      public QGraphicsPixmapItem
{
  W_OBJECT(QGraphicsPixmapToggle)
  Q_INTERFACES(QGraphicsItem)

  const QPixmap m_pressed, m_released;
  bool m_toggled{};

public:
  QGraphicsPixmapToggle(
      QPixmap pressed,
      QPixmap released,
      QGraphicsItem* parent);

  void toggle();
  void setState(bool toggled);

public:
  void toggled(bool arg_1) W_SIGNAL(toggled, arg_1);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};

struct DefaultGraphicsSliderImpl
{
  struct DoubleSpinboxWithEnter final : public QDoubleSpinBox
  {
  public:
    using QDoubleSpinBox::QDoubleSpinBox;

  public:
    bool event(QEvent* event) override
    {
      if (event->type() == QEvent::ShortcutOverride)
      {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return)
        {
          editingFinished();
        }
      }
      return QDoubleSpinBox::event(event);
    }
  };

  template <typename T>
  static void mousePressEvent(T& self, QGraphicsSceneMouseEvent* event)
  {
    if (self.isInHandle(event->pos()))
    {
      self.m_grab = true;
    }

    const auto srect = self.sliderRect();
    double curPos
        = ossia::clamp(event->pos().x(), 0., srect.width()) / srect.width();
    if (curPos != self.m_value)
    {
      self.m_value = curPos;
      self.valueChanged(self.m_value);
      self.sliderMoved();
      self.update();
    }

    event->accept();
  }

  template <typename T>
  static void mouseMoveEvent(T& self, QGraphicsSceneMouseEvent* event)
  {
    if (self.m_grab)
    {
      const auto srect = self.sliderRect();
      double curPos
          = ossia::clamp(event->pos().x(), 0., srect.width()) / srect.width();
      if (curPos != self.m_value)
      {
        self.m_value = curPos;
        self.valueChanged(self.m_value);
        self.sliderMoved();
        self.update();
      }
    }
    event->accept();
  }

  template <typename T>
  static void mouseReleaseEvent(T& self, QGraphicsSceneMouseEvent* event)
  {
    if (self.m_grab)
    {
      double curPos
          = ossia::clamp(event->pos().x() / self.sliderRect().width(), 0., 1.);
      if (curPos != self.m_value)
      {
        self.m_value = curPos;
        self.valueChanged(self.m_value);
        self.update();
      }
      self.m_grab = false;
    }
    self.sliderReleased();
    event->accept();
  }

  template <typename T>
  static void mouseDoubleClickEvent(T& self, QGraphicsSceneMouseEvent* event)
  {
    auto w = new DoubleSpinboxWithEnter;
    w->setRange(self.map(self.min), self.map(self.max));

    w->setDecimals(6);
    w->setValue(self.map(self.m_value * (self.max - self.min) + self.min));
    auto obj = self.scene()->addWidget(
        w, Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    obj->setPos(event->scenePos());
    w->setFocus();

    QObject::connect(
        w,
        SignalUtils::QDoubleSpinBox_valueChanged_double(),
        &self,
        [=, &self](double v) {
          self.m_value = (self.unmap(v) - self.min) / (self.max - self.min);
          self.valueChanged(self.m_value);
          self.sliderMoved();
          self.update();
        });
    QObject::connect(w, &QDoubleSpinBox::editingFinished, &self, [=, &self] {
      self.scene()->removeItem(obj);
      obj->deleteLater();
    });
  }
};

class SCORE_LIB_BASE_EXPORT QGraphicsSlider final : public QObject,
                                                    public QGraphicsItem
{
  W_OBJECT(QGraphicsSlider)
  Q_INTERFACES(QGraphicsItem)
  friend struct DefaultGraphicsSliderImpl;

  double m_value{};
  QRectF m_rect{};

public:
  double min{}, max{};

private:
  bool m_grab{};

public:
  QGraphicsSlider(QGraphicsItem* parent);

  static double map(double v) { return v; }
  static double unmap(double v) { return v; }

  void setRect(const QRectF& r);
  void setValue(double v);
  double value() const;

  bool moving = false;

public:
  void valueChanged(double arg_1) W_SIGNAL(valueChanged, arg_1);
  void sliderMoved() W_SIGNAL(sliderMoved);
  void sliderReleased() W_SIGNAL(sliderReleased);

private:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

  QRectF boundingRect() const override;
  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override;
  bool isInHandle(QPointF p);
  double getHandleX() const;
  QRectF sliderRect() const;
  QRectF handleRect() const;
};

class SCORE_LIB_BASE_EXPORT QGraphicsLogSlider final : public QObject,
                                                       public QGraphicsItem
{
  W_OBJECT(QGraphicsLogSlider)
  Q_INTERFACES(QGraphicsItem)
  friend struct DefaultGraphicsSliderImpl;

  double m_value{};
  QRectF m_rect{};

public:
  double min{}, max{};

private:
  bool m_grab{};

public:
  QGraphicsLogSlider(QGraphicsItem* parent);

  void setRect(const QRectF& r);
  void setValue(double v);
  double value() const;

  static double map(double v) { return std::exp2(v); }
  static double unmap(double v) { return std::log2(v); }

  bool moving = false;

public:
  void valueChanged(double arg_1) W_SIGNAL(valueChanged, arg_1);
  void sliderMoved() W_SIGNAL(sliderMoved);
  void sliderReleased() W_SIGNAL(sliderReleased);

private:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
  QRectF boundingRect() const override;
  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override;
  bool isInHandle(QPointF p);
  double getHandleX() const;
  QRectF sliderRect() const;
  QRectF handleRect() const;
};

class SCORE_LIB_BASE_EXPORT QGraphicsIntSlider final : public QObject,
                                                       public QGraphicsItem
{
  W_OBJECT(QGraphicsIntSlider)
  Q_INTERFACES(QGraphicsItem)
  friend struct DefaultGraphicsSliderImpl;
  QRectF m_rect{};
  int m_value{}, m_min{}, m_max{};
  bool m_grab{};

public:
  QGraphicsIntSlider(QGraphicsItem* parent);

  void setRect(const QRectF& r);
  void setValue(int v);
  void setRange(int min, int max);
  int value() const;

  bool moving = false;

public:
  void valueChanged(int arg_1) W_SIGNAL(valueChanged, arg_1);
  void sliderMoved() W_SIGNAL(sliderMoved);
  void sliderReleased() W_SIGNAL(sliderReleased);

private:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  QRectF boundingRect() const override;
  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override;
  bool isInHandle(QPointF p);
  double getHandleX() const;
  QRectF sliderRect() const;
  QRectF handleRect() const;
};

class SCORE_LIB_BASE_EXPORT QGraphicsComboSlider final : public QObject,
                                                         public QGraphicsItem
{
  W_OBJECT(QGraphicsComboSlider)
  Q_INTERFACES(QGraphicsItem)
  friend struct DefaultGraphicsSliderImpl;
  QRectF m_rect{};

public:
  QStringList array;

private:
  int m_value{};
  bool m_grab{};

public:
  template <std::size_t N>
  QGraphicsComboSlider(
      const std::array<const char*, N>& arr,
      QGraphicsItem* parent)
      : QGraphicsItem{parent}
  {
    array.reserve(N);
    for (auto str : arr)
      array.push_back(str);

    this->setAcceptedMouseButtons(Qt::LeftButton);
  }

  QGraphicsComboSlider(QStringList arr, QGraphicsItem* parent)
      : QGraphicsItem{parent}, array{std::move(arr)}
  {
    this->setAcceptedMouseButtons(Qt::LeftButton);
  }

  QGraphicsComboSlider(QGraphicsItem* parent);

  void setRect(const QRectF& r);
  void setValue(int v);
  int value() const;

  bool moving = false;

public:
  void valueChanged(int arg_1) W_SIGNAL(valueChanged, arg_1);
  void sliderMoved() W_SIGNAL(sliderMoved);
  void sliderReleased() W_SIGNAL(sliderReleased);

private:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  QRectF boundingRect() const override;
  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override;
  bool isInHandle(QPointF p);
  double getHandleX() const;
  QRectF sliderRect() const;
  QRectF handleRect() const;
};
} // namespace score
