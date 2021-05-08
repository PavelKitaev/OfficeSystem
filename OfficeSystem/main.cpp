#include "mainform.h"
#include <QApplication>
#include <QPushButton>

Log lg; //Логирование
QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL"); //Создаем подключение к базе данных
SettingFile *settingfile = new SettingFile(); //Редактирования файла настроек

void EditSetting()
{
    //Заполнение данных через диалоговые окна
    bool ok;
    QString dBName = "DatabaseName = [";
    dBName += QInputDialog::getText(0,"Укажите имя базы данных", "DataBaseName:", QLineEdit::Normal, "office_system", &ok);
    dBName += "]\n";
    if (!ok)
    {
        return;
    }
    else
    {
        settingfile->EditSettingFile(dBName); //Функция внесения изменений в файл
    }

    QString hostName = "HostName = [";
    hostName += QInputDialog::getText(0, "Укажите имя хоста", "HostName:", QLineEdit::Normal, "127.0.0.1", &ok);
    hostName += "]\n";
    if (!ok)
    {
        return;
    }
    else
    {
        settingfile->EditSettingFile(hostName);
    }

    QString port = "Port = [";
    port += QInputDialog::getText(0, "Укажите порт", "Port:", QLineEdit::Normal, "5432", &ok);
    port += "]\n";
    if (!ok)
    {
        return;
    }
    else
    {
        settingfile->EditSettingFile(port);
    }

    QString userName = "UserName = [";
    userName += QInputDialog::getText(0, "Укажите имя пользователя", "UserName:", QLineEdit::Normal, "postgres", &ok);
    userName += "]\n";
    if (!ok)
    {
        return;
    }
    else
    {
        settingfile->EditSettingFile(userName);
    }

    QString pass = "Password = [";
    pass += QInputDialog::getText(0, "Укажите пароль", "UserName:", QLineEdit::Normal, "1", &ok);
    pass += "]\n";
    if (!ok)
    {
        return;
    }
    else
    {
        settingfile->EditSettingFile(pass);
    }
}

void ConnectedDataBase()
{
    QString temp, dBName, hostName, port, userName, pass;

    //Получение данных для подключения из файла
    dBName = settingfile->GetParameter("DatabaseName"); //Имя базы данных
    hostName = settingfile->GetParameter("HostName");   //Адрес базы
    port = settingfile->GetParameter("Port");           //Порт
    userName = settingfile->GetParameter("UserName");   //Имя пользователя
    pass = settingfile->GetParameter("Password");       //Пароль пользователя

    //Если данные не пустые - заполняем параметры подключения
    if (dBName != "" && hostName != "" && port != "" && userName != "" && pass != "")
    {
        db.setDatabaseName(dBName);
        db.setHostName(hostName);
        db.setPort(port.toInt());
        db.setUserName(userName);
        db.setPassword(pass);
    }
    else //Если данные пустные
    {
        lg.WriteMsg("#L004 - Файл setting.ini поврежден. Требуется восстановление."); //Запишем ошибку в лог-файл

        //Сообщение с ошибкой
        QMessageBox msg;
        msg.setText("Файл настроек пуст или поврежден. Введите корректные данные");
        msg.exec();

        EditSetting(); //Ввод вновых данных
        ConnectedDataBase(); //Повторное подключение
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    bool connected = false;

    while(!connected)
    {
        ConnectedDataBase(); //Подключение к базе данных

        if (!db.open()) //Если соединение с базой нет
        {
            lg.WriteMsg(db.lastError().text()); //Запишем ошибку в лог-файл

            //Выведем информацию на экран
            QMessageBox msg;
            msg.setText(db.lastError().text() + "\n\nОшибка при подключении к базе данных. Обратитесь к администратору.");
            QPushButton *edit = msg.addButton(("Изменить данные подключения"), QMessageBox::ActionRole);
            QPushButton *exit = msg.addButton(("Закрыть программу"), QMessageBox::ActionRole);
            msg.exec();

            if (msg.clickedButton() == exit)
            {
                return 0;
            }

            if (msg.clickedButton() == edit)
            {
                EditSetting(); //Изменение данных подключения
            }
        }
        else //Если подключение к базе есть - открываем новую форму
        {
            connected = true;
            MainForm w; //Форма с авторизацией
            w.setWindowTitle("OfficeSystem"); //Заголовок
            w.showMaximized(); //На весь экран
            return a.exec();
        }
    }
    return 0;
}
