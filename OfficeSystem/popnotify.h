#ifndef POPNOTIFY_H
#define POPNOTIFY_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QTimer>

class PopNotify : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float popupOpacity READ getPopupOpacity WRITE setPopupOpacity)   // Свойство полупрозрачности

    void setPopupOpacity(float opacity);                                        //Установка прозначности
    float getPopupOpacity() const;                                              //Получение значения прозрачности

public:
    explicit PopNotify(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);                                       //Отрисовка фона

public slots:
    void setPopupText(const QString& text, int size_font = 16 );               //Установка текста в уведомление
    void show(QWidget *_this);                                                 //Собственный метод показа виджета (для преварительной настройки анимации)

private slots:
    void hideAnimation();                                                      //Слот для запуска анимации скрытия
    void hide();                                                               //Проверка, можно ли полностью скрыть уведомление

private:
    QLabel label;                                                              //Label с сообщением
    QGridLayout layout;                                                        //Размещение для лейбла
    QPropertyAnimation animation;                                              //Свойство анимации для всплывающего сообщения
    float popupOpacity;                                                        //Свойства полупрозрачности виджета
    QTimer *timer;                                                             //Таймер, по которому виджет будет скрыт
};

#endif // POPNOTIFY_H
