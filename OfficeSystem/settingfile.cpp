#include "settingfile.h"

SettingFile::SettingFile()
{
    file_name = "setting.ini"; //Название файла
    file.setFileName(file_name);
}

void SettingFile::EditSettingFile(QString _data)
{
    QStringList data;   //Для хранения информации из файла
    int str = -1;       //Номер изменяемой строки

    if (file.open(QIODevice::ReadOnly |QIODevice::Text)) //Открываем файл на чтение
    {
        QString FileText;
        while(!file.atEnd()) //Проходим файл и запилываем все строки в СтринжЛист Дата
        {
            FileText = file.readLine();
            data += FileText;
        }
    }

    file.close(); //Закрываем файл

    //Узнаем, была ли строка в файле
    QString searchString;
    for (int i = 0; i < _data.length(); i++) //Считываем заголовок в searchString
    {
        if (_data[i] != " ") //Читам до первого пробела
        {
            searchString += _data[i];
        }
        else //Нашли пробел - выходим
        {
            break;
        }
    }

    str = data.indexOf(QRegExp("^" + searchString + ".+")); //Находим индекс строки, которую нужно изменить

    if (str != -1) //Если строка найдена, ее нужно перезаписать
    {
        data[str] = _data;
    }
    else  //Если не найдена - добавить
    {
        data += _data;
    }

    //Очищаем файл
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.close();
    }

    //Открываем файл на запись и переносим изменненый СтринжЛист Дата в файл
    if (file.open(QIODevice::WriteOnly |QIODevice::Text))
    {
        for (int i = 0; i < data.length(); i++)
        {
            file.write(data[i].toUtf8());
        }

        file.close(); //Закрываем файл
    }
}

QString SettingFile::GetParameter(QString _data) //Получение параметра из файла
{
    QString res, temp;

    if (file.open(QIODevice::ReadOnly |QIODevice::Text))                //Открываем файл на чтение
    {
        while(!file.atEnd())                                            //Идем до конца
        {
            temp = file.readLine();                                     //Получаем строку

            if (temp.indexOf(_data) == 0)                               //Если в полученной строке есть нужное включение
            {
                for (int i = 0; i < temp.length(); i++)                 //Достаем данные
                {
                    if (temp[i] == "[")                                 //Идем до открывающейся скобки
                    {
                        for (i = i + 1; i < temp.length(); i++)
                        {
                            if (temp[i] == "]")                         //Дошли до закр. скобки - выходим
                            {
                                break;
                            }
                            res += temp[i];                             //Записываем данные
                        }
                    }
                }

                break;
            }
        }
    }

    file.close(); //Закрываем файл
    return res;
}
