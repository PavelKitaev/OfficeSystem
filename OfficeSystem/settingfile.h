#ifndef SETTINGFILE_H
#define SETTINGFILE_H

#include <QFile>           //Для работы с файлом

class SettingFile
{
private:
    QString file_name;     //Имя файла
    QFile file;            //Переменная для открытия файла

public:
    SettingFile();
    ~SettingFile();

    void EditSettingFile(QString data); //Редактирование файла
    QString GetParameter(QString data); //Получение параметра из файла
};

#endif // SETTINGFILE_H
