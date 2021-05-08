#ifndef OFFICESYSTEM_H
#define OFFICESYSTEM_H

#include <QWidget>

class OfficeSystem
{
public:
    OfficeSystem() {}
    static bool CheckPassword(QString _pass);                                                //Проверка пароля на безопасность
    static QString GetStyleSheetButton();                                                    //Стиль кнопки
    static QString GetStyleSheetButtonUserOffline();                                         //Стиль кнопки для офлайн пользователя
    static QString GetStyleSheetButtonUnread();                                              //Стиль кнопки для пользователя с непрочитанным сообщением
    static QString GetStyleSheetButtonForIcon();                                             //Стиль кнопки под иконку
    static QString GetStyleSheetLineEdit();                                                  //Стиль LineEdit
    static QString GetStyleSheetVerticalScrollBar();                                         //Стиль скролл бара вертикального
    static QString GetStyleSheetHorizontalScrollBar();                                       //Стиль скролл бара горизонтального
    static QString GetStyleSheetLabel(QString colorText = "rgb(1, 140, 234)");               //Стиль label
    static QString GetStyleSheetWidget(QString border = "all", QString color = "white");     //Стиль виджета
    static QString GetStyleSheetTextEdit();                                                  //Стиль TextEdit
    static QString GetStyleSheetTextBrowser();                                               //Стиль TextBrowser
    static QString GetStyleSheetHorizontalHeaderView();                                      //Стиль горизонтальной шипки таблицы
    static QString GetStyleSheetVerticalHeaderView();                                        //Стиль вертикальной шапки таблицы
    static QString GetStyleSheetTableView();                                                 //Стиль таблицы
    static QString GetStyleSheetDialog();                                                    //Стиль для диалогововых окон
};

#endif // OFFICESYSTEM_H
               
