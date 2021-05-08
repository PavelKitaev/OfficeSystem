#include "popnotify.h"
#include <QPainter>

PopNotify::PopNotify(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint |        //Отключаем оформление окна
                   Qt::Tool |                       //Отменяем показ в качестве отдельного окна
                   Qt::WindowStaysOnTopHint);       //Устанавливаем поверх всех окон
    setAttribute(Qt::WA_TranslucentBackground);     //Указываем, что фон будет прозрачным
    setAttribute(Qt::WA_ShowWithoutActivating);     //При показе, виджет не получается фокуса автоматически

    animation.setTargetObject(this);                //Устанавливаем целевой объект анимации
    animation.setPropertyName("popupOpacity");      //Устанавилваем анимируемое свойство
    connect(&animation, &QAbstractAnimation::finished, this, &PopNotify::hide); //Подключаем сигнал окончания анимации к слоту скрытия


    // Настройка текста уведомления
    label.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter); // Устанавилваем по центру
    // И настраиваем стили
    label.setStyleSheet("QLabel {color: white;\
                                 margin-top: 6px;\
                                 margin-bottom: 6px;\
                                 margin-left: 10px;\
                                 margin-right: 10px;}");

    layout.addWidget(&label, 0, 0); //Производим установку текста в менеджер
    setLayout(&layout); //Помещаем в виджет

    timer = new QTimer(); //По сигналу таймера будет произведено скрытие уведомления, если оно видимо
    connect(timer, &QTimer::timeout, this, &PopNotify::hideAnimation);
}

void PopNotify::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); //Включаем сглаживание

    //Внутренняя геометрия виджета уведомления по содержимому
    QRect roundedRect;
    roundedRect.setX(rect().x() + 5);
    roundedRect.setY(rect().y() + 5);
    roundedRect.setWidth(rect().width() - 10);
    roundedRect.setHeight(rect().height() - 10);

    painter.setBrush(QBrush(QColor(0,0,0,180))); //Кисть настраиваем на чёрный цвет в режиме полупрозрачности 180 из 255
    painter.setPen(Qt::NoPen); // Край уведомления не будет выделен
    painter.drawRoundedRect(roundedRect, 10, 10); //Отрисовываем фон с закруглением краёв в 10px
}

void PopNotify::setPopupText(const QString &text, int size_font)
{
    QFont font = label.font();
    font.setPixelSize(size_font);
    label.setFont(font);
    label.setText(text);    //Устанавилваем текст в Label
    adjustSize();           //С пересчётом размеров уведомления
}

void PopNotify::show(QWidget *_this)
{
    setWindowOpacity(0.0);  //Устанавливаем прозрачность в ноль

    animation.setDuration(150);     //Длительность анимации
    animation.setStartValue(0.0);   //Стартовое значение будет 0 (полностью прозрачный виджет)
    animation.setEndValue(1.0);     //Конечное - полностью непрозрачный виджет
    int x = _this->geometry().x() + _this->geometry().width();
    int y = _this->geometry().y() + _this->geometry().height();
    setGeometry(x - this->geometry().width(), y - this->geometry().height(), width(), height());

    QWidget::show();                //Отображаем виджет, который полностью прозрачен

    animation.start();              //Запускаем анимацию
    timer->start(3000);             //Стартуем таймер, который запустит скрытие уведомления через 3 секунды
}

void PopNotify::hideAnimation()
{
    timer->stop();                  //Останавлвиваем таймер
    animation.setDuration(1000);    //Длительность анимации
    animation.setStartValue(1.0);   //Стартовое значение будет 1 (полностью непрозрачный виджет)
    animation.setEndValue(0.0);     //Конечное - полностью прозрачный виджет
    animation.start();              //И запускаем анимацию
}

void PopNotify::hide()
{
    //Если виджет прозрачный, то скрываем его
    if(getPopupOpacity() == 0.0){
        QWidget::hide();
    }
}

void PopNotify::setPopupOpacity(float opacity)
{
    popupOpacity = opacity; //Установка прозрачности
    setWindowOpacity(opacity);
}

float PopNotify::getPopupOpacity() const
{
    return popupOpacity;
}
