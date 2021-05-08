#include "officesystem.h"

bool OfficeSystem::CheckPassword(QString _pass)
{
    bool res = true;
    int size = _pass.length(); //Длина пароля

    //Проверяем длину
    if (_pass.length() < 8)
    {
        res = false;
    }
    else
    {
        int symbol = 0; //Спец символы
        int letterSmall = 0; //Буквы маленькие
        int letterBig = 0; //Буквы большие
        int number = 0; //Цифры

        for (int i = 0; i < size; i++)
        {
            //Проверка на большие буквы по таблице ASCII (латинские и русские)
                //С 65 по 90 - латинкие большие
                //С 128 по 159 - русские большие
            if ((_pass[i] >= 65 && _pass[i] <= 90) || (_pass[i] >= 128 && _pass[i] <= 159))
            {
                letterBig++;
            }

            //Проверка на маленькие буквы
                //С 97 по 122 - латинские маленькие
                //С 160 по 175 - русские маленькие
            else if ((_pass[i] >= 97 && _pass[i] <= 122) || (_pass[i] >= 160 && _pass[i] <= 175) || (_pass[i] >= 224 && _pass[i] <= 241))
            {
                letterSmall++;
            }

            //Проверка на цифры
            else if (_pass[i] >= 48 && _pass[i] <= 57)
            {
                number++;
            }

            //Спец. символы
            else
            {
                symbol++;
            }
        }

        //Переводим в проценты
        letterSmall = (letterSmall * 100) / size;
        letterBig = (letterBig * 100) / size;
        number = (number * 100) / size;
        symbol = (symbol * 100) / size;

        //Пароль должен содержать маленькие буквы, большие и цифры. Спец.символы упускаем.
        if (letterBig == 0 || letterSmall == 0 || number == 0)
        {
            res = false;
        }
    }

    return res;
}

QString OfficeSystem::GetStyleSheetButton()
{
    return "QPushButton:enabled { background-color: white;\
                                  font: lato;\
                                  border-style: solid;\
                                  border-width: 2px;\
                                  border-radius: 15px;\
                                  border-color: rgb(1, 140, 234);\
                                  font: 14pt;\
                                  padding: 6px;\
                                  color: rgb(1, 140, 234); }\
            QPushButton:disabled { background-color: rgb(240, 240, 240);\
                                   font: lato;\
                                   border-style: solid;\
                                   border-width: 2px;\
                                   border-radius: 15px;\
                                   border-color: rgb(4, 69, 87);\
                                   font: 14pt;\
                                   padding: 6px;\
                                   color: rgb(4, 69, 87); }\
            QPushButton:default { border-color: navy; }\
            QPushButton:hover { background-color: rgb(246, 246, 246); }\
            QPushButton:pressed { background-color: rgb(240, 240, 240); }";
}

QString OfficeSystem::GetStyleSheetButtonUserOffline()
{
    return "QPushButton:enabled { background-color: white;\
                                  font: lato;\
                                  border-style: solid;\
                                  border-width: 2px;\
                                  border-radius: 15px;\
                                  border-color: rgb(4, 69, 87);\
                                  font: 14pt;\
                                  padding: 6px;\
                                  color: rgb(4, 69, 87); }\
            QPushButton:default { border-color: navy; }\
            QPushButton:hover { background-color: rgb(246, 246, 246); }\
            QPushButton:pressed { background-color: rgb(240, 240, 240); }";
}

QString OfficeSystem::GetStyleSheetButtonUnread()
{
    return "QPushButton:enabled{ background-color: white;\
                                 font: lato;\
                                 border-style: solid;\
                                 border-width: 2px;\
                                 border-radius: 15px;\
                                 border-color: rgb(234, 95, 1);\
                                 font: 14pt;\
                                 padding: 6px;\
                                 color: rgb(234, 95, 1); }\
            QPushButton:default { border-color: navy; }\
            QPushButton:hover { background-color: rgb(246, 246, 246); }\
            QPushButton:pressed { background-color: rgb(240, 240, 240); }";
}

QString OfficeSystem::GetStyleSheetButtonForIcon()
{
    return "QPushButton:enabled { background-color: none;\
                                  border-style: none; font: lato;}\
            QPushButton:disabled { background-color: none;\
                                   border-style: none; font: lato;}\
            QPushButton:default { border-color: navy; }\
            QPushButton:pressed { padding-top: 3px; }";
}

QString OfficeSystem::GetStyleSheetLineEdit()
{
    return "QLineEdit { background-color: white;\
                        border-style: solid;\
                        border-width: 10px;\
                        border-color: white;\
                        border-radius: 15px;\
                        font:  14pt;\
                        color: rgb(1, 140, 234); }";
}

QString OfficeSystem::GetStyleSheetVerticalScrollBar()
{
    return "QScrollBar:vertical { border: 2px solid rgb(1, 140, 234); \
                                 background: none; \
                                 width: 15px; \
                                 margin: 22px 0 22px 0; }\
            QScrollBar::handle:vertical { background: rgb(193, 220, 248); \
                                          min-height: 20px; }\
            QScrollBar::add-line:vertical { border: 2px solid rgb(1, 140, 234); \
                                            background: none; \
                                            height: 20px; \
                                            subcontrol-position: bottom; \
                                            subcontrol-origin: margin; }\
            QScrollBar::sub-line:vertical { border: 2px solid rgb(1, 140, 234); \
                                            background: none; height: 20px; \
                                            subcontrol-position: top; \
                                            subcontrol-origin: margin; }\
            QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { border: 2px solid rgb(1, 140, 234); \
                                                                             width: 3px; \
                                                                             height: 3px; \
                                                                             background: white; }\
            QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }";
}

QString OfficeSystem::GetStyleSheetHorizontalScrollBar()
{
    return "QScrollBar:horizontal { border: 2px solid rgb(1, 140, 234); \
                                    background: none; \
                                    height: 15px; \
                                    margin: 0px 22px 0px 22px; }\
            QScrollBar::handle:horizontal { background: rgb(193, 220, 248); \
                                            min-width: 20px; }\
            QScrollBar::add-line:horizontal { border: 2px solid rgb(1, 140, 234); \
                                              background: none; \
                                              width: 20px; \
                                              subcontrol-position: right; \
                                              subcontrol-origin: margin; }\
            QScrollBar::sub-line:horizontal { border: 2px solid rgb(1, 140, 234); \
                                              background: none; \
                                              width: 20px; \
                                              subcontrol-position: left; \
                                              subcontrol-origin: margin; }\
            QScrollBar::left-arrow:horizontal, QScrollBar::right-arrow:horizontal { border: 2px solid rgb(1, 140, 234); \
                                                                                    width: 3px; \
                                                                                    height: 3px; \
                                                                                    background: white; }\
            QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { background: none; }";
}

QString OfficeSystem::GetStyleSheetWidget(QString border, QString color)
{
    QString res;

    if (border == "top") //Закругление только сверху
    {
        res = "background-color: " + color + "; border-top-right-radius: 20px; border-top-left-radius: 20px;";
    }

    if (border == "bottom") //Закругление только снизу
    {
        res =  "background-color: " + color + "; border-bottom-right-radius: 20px; border-bottom-left-radius: 20px;";
    }

    if (border == "all") //Закругление везде
    {
        res = "background-color: " + color + "; border-radius: 20px;";
    }

    return res;

}

QString OfficeSystem::GetStyleSheetTextEdit()
{
    return "QTextEdit { border-width: 10px; \
                        border-style: solid; \
                        border-color: white; \
                        background-color: white; \
                        border-radius: 20px; \
                        font: lato; \
                        font-size: 14px; \
                        color: rgb(4, 69, 87); }";
}

QString OfficeSystem::GetStyleSheetLabel(QString colorText)
{
    return "QLabel { color: " + colorText + "; \
                     font: lato; \
                     font-size: 18px; }";
}

QString OfficeSystem::GetStyleSheetTextBrowser()
{
    return "QTextBrowser { border-width: 10px; \
                           border-style: solid; \
                           border-color: white; \
                           border-background-color: white; \
                           border-bottom-right-radius: 20px; \
                           border-bottom-left-radius: 20px; \
                           font: lato; \
                           font-size: 16px; \
                           color: rgb(4, 69, 87); }";
}

QString OfficeSystem::GetStyleSheetHorizontalHeaderView()
{
    return "QHeaderView { height: 30px;\
                          background: white;\
                          color: rgb(1, 140, 234);\
                          font: Lato;\
                          font-size: 15pt;\
                          border: none;\
                          border-bottom: 2px solid rgb(1, 140, 234); }";
}

QString OfficeSystem::GetStyleSheetVerticalHeaderView()
{
    return "QHeaderView { height: 30px;\
                          background: white;\
                          color: rgb(1, 140, 234);\
                          font: Lato;\
                          font-size: 15pt;\
                          border: none;\
                          border-bottom: 2px solid rgb(1, 140, 234); }\
           QHeaderView::section:vertical { width: 25px;\
                                           background: white;\
                                           border: 0px solid white;\
                                           border-right: 2px solid rgb(1, 140, 234);\
                                           color: rgb(1, 140, 234);\
                                           font-size: 15pt; }";
}

QString OfficeSystem::GetStyleSheetTableView()
{
    return "QTableView { background: white;\
                         font-size: 14pt;\
                         border-style: solid;\
                         border-width: 10px;\
                         border-color: white;\
                         border-radius: 15px;\
                         color: rgb(4, 69, 87); }\
           QTableView QTableCornerButton::section { background: white;\
                                                    border: none;}\
           QTableView { selection-color: black;\
                        selection-background-color: rgba(76, 155, 235, 50);}";
}

QString OfficeSystem::GetStyleSheetDialog()
{
    return "QDialog QPushButton:enabled { background-color: white;\
                                          font: lato;\
                                          border-style: solid;\
                                          border-width: 2px;\
                                          border-radius: 15px;\
                                          border-color: rgb(76, 155, 235);\
                                          font: 14pt;\
                                          padding: 6px;\
                                          color: rgb(76, 155, 235); }\
                    QPushButton:disabled { background-color: rgb(240, 240, 240);\
                                           font: lato;\
                                           border-style: solid;\
                                           border-width: 2px;\
                                           border-radius: 15px;\
                                           border-color: rgb(100, 100, 100);\
                                           font: 14pt;\
                                           padding: 6px;\
                                           color: rgb(100, 100, 100); }\
                    QPushButton:default { border-color: navy; }\
                    QPushButton:hover { background-color: rgb(246, 246, 246); }\
                    QPushButton:pressed { background-color: rgb(240, 240, 240); }";
}
