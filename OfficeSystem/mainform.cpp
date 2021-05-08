#include "mainform.h"
#include "ui_mainform.h"
#include "admform.h"
#include "mngform.h"
#include "scrform.h"
#include "log.h"

MainForm::MainForm(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainForm)
{
    ui->setupUi(this);
    bg.load(":/image/background.png"); //Устанавливаем фон

    popNotify = new PopNotify(); //Уведомления
    settingFile = new SettingFile();    //Файл настроек
    lg = new Log(); //Журнализирование
    serverOk = true; //Открытие новой формы с сервером/без
    Connection = NULL; //Будущее подключение к серверу
    CheckTableAccounts(); //Проверка существования таблицы и ее создание, в случае отсутствия

    //Первичная проверка капс лока и раскладки. Далее за это отвечает событие нажатия клавишей
    if (CheckCapsLock())
    {
        ui->lb_caps->setText("   Включен CapsLock");
    }

    threadId = GetWindowThreadProcessId(NULL, NULL);
    hkl = GetKeyboardLayout(threadId);
    code = LOWORD(hkl);

    if (code == 1033) //Англ
    {
        ui->lb_layout->setText("   EN");
    }
    else if (code == 1049) //Русская раскладка
    {
        ui->lb_layout->setText("   RU");
    }
    else
    {
        ui->lb_layout->setText("");
    }
}

MainForm::~MainForm()
{
    if (Connection != NULL) //Если подключеник с серверу активно - посылаем команду отключения
    {
        std::string msg = "DISCONNECTING_CLIENT::" + login.toStdString();
        int msg_size = msg.size();

        send(Connection, (char*)&msg_size, sizeof(int), 0);
        send(Connection, msg.c_str(), msg_size, 0);

        Sleep(2000); //Ждем, что бы команда была точно доставлена
    }
    delete ui;
}

void MainForm::CheckTableAccounts() //Проверка таблицы
{
    bool res = false;

    query.exec("SELECT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name = 'of_user_accounts')"); //Проверка существования

    if (query.next())
    {
        res = query.value(0).toBool();
    }

    if (!res) //Если таблицы нет - создаем
    {
        query.exec("CREATE TABLE IF NOT EXISTS of_user_accounts (id_user bigserial NOT NULL, login character varying(15),\
                         password character varying(30), question text, answer text, fio text, role character varying(15),\
                         dept text, PRIMARY KEY(login))");

        query.exec("INSERT INTO of_user_accounts (login, password, fio, role) VALUES ('FirstUser', 'FirstPassword', 'Подлежит удалению!', 'Администратор')");

        QMessageBox msg; //Приветственное сообщение
        msg.setText("Офисная система приветствует Вас!\n"
                    "Это первый запуск программы, поэтому в базе данных была создана таблица с учетной записью администратора. "
                    "Войдите, используя данные, указанные ниже и добавьте новых пользователей. \n\nИмя пользователя: FirstUser \nПароль: FirstPassword"
                    "\n\nОбязательно удалите эту учетную запись!");
             msg.exec();
    }
}

void MainForm::EditSettingFile() //Редактирование настроек
{
    bool ok;
    QString addrServer = "Address_server = [";
    addrServer += QInputDialog::getText(0,"Укажите адрес сервера", "Address_server:",
                                            QLineEdit::Normal, "127.0.0.1", &ok);
    addrServer += "]\n";
    if (!ok)
    {
        return;
    }
    else
    {
        settingFile->EditSettingFile(addrServer);
    }

    QString portServer = "Port_server = [";
    portServer += QInputDialog::getText(0, "Укажите порт сервера", "Port_server:",
                                         QLineEdit::Normal, "1111", &ok);
    portServer += "]\n";
    if (!ok)
    {
        return;
    }
    else
    {
        settingFile->EditSettingFile(portServer);
    }
}

void MainForm::SuccessLogin(QString _login, QString role)
{
    this->hide(); //Скрываем форму авторизации
    lg->WriteMsg("Авторизация клиента " +_login + "."); //Логирование

    if (role == "Администратор") //Инициализируем форму основной программы для роли "Администратор"
    {
        AdmForm *af = new AdmForm(Connection, _login);      //Создание формы
        af->setMinimumSize(1500, 800);                      //Минимальные размеры
        af->showMaximized();                                //Показать развернуто
        af->setWindowTitle("OfficeSystem: Administrator");  //Заголовок
    }
    else if (role == "Пользователь") //Инициализируем форму основной программы для роли "Менеджер"
    {
        MngForm *mf = new MngForm(Connection, _login);
        mf->setMinimumSize(1500, 800);
        mf->showMaximized();
        mf->setWindowTitle("OfficeSystem: User");
    }
    else if (role == "Охранник") //Инициализируем форму основной программы для роли "Охрана"
    {
        ScrForm *sf = new ScrForm(Connection, _login);
        sf->setMinimumSize(1500, 800);
        sf->showMaximized();
        sf->setWindowTitle("OfficeSystem: Security");
    }
    else
    {
        popNotify->setPopupText("Ошибка подключения. Данный вид доступа недоступен");
        popNotify->show(this);;
    }
}

int MainForm::ConnectionToServer(QString address, QString  port)
{
    //Инициализация Библиотеки Сокетов
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);

    if (WSAStartup(DLLVersion, &wsaData) != 0)
    {
        lg->WriteMsg("#C001 - Несоответствие сетевых библиотек.");
        exit(1);
    }

    //Создание сокета
    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr(address.toUtf8()); //Адрес сервера
    addr.sin_port = htons(port.toShort());              //Порт сервера
    addr.sin_family = AF_INET;                          //Сокет интернета

    Connection = socket(AF_INET, SOCK_STREAM, 0);

    int res = ::connect(Connection, (SOCKADDR*)&addr, sizeof(addr)); //Подключение

    if (res != 0)
    {
        WSACleanup();
    }

    return res;
}


void MainForm::on_bt_enter_clicked() //Кнопка "Вход"
{
    login = ui->le_login->text();       //Логин, введеный в поле ввода
    QString pass = ui->le_pass->text(); //Пароль
    QString role;                       //Вид доступа

    if ((login == nullptr || login == " ") || (pass == nullptr || pass == " ")) //Поверхностная проверка корректности заполнения полей
    {
        popNotify->setPopupText("Ошибка заполнения");
        popNotify->show(this);
    }
    else //Проверяем наличие логина в базе
    {
        query.exec("SELECT password FROM of_user_accounts WHERE login = '" + login + "'"); //Делаем запрос к БД
        query.next();

        QString passBd = query.value(0).toString(); //Будет равен "" если записи с логином нет в таблице

        if (passBd != "") //Если логин есть в базе, то проверяем пароль
        {
            if (pass == passBd) //Если пароль совпал
            {
                while (true) //Подключаемся к серверу
                {
                    QString address = settingFile->GetParameter("Address_server"); //Получаем адрес из файла
                    QString port = settingFile->GetParameter("Port_server");       //Поулчаем порт из файла

                    int con = ConnectionToServer(address, port); //Попытка подключения. Возвращает 0 при успехе, иначе -1
                    if ( con != 0) //Если подключение неудачное
                    {
                        //Окно с ошибкой и вариантами решения проблемы
                        int control = ErrorConnectionServer("#L002 - Ошибка подключения к серверу. Отсутствуют данные для подключения либо сервер недоступен.");

                        if ( control == 1 || control == 3) //Если данные были измененены, или была открыта справка
                        {
                            continue;
                        }
                        else if (control == 2) //Если выбран вариант продолжения без сервера
                        {
                            serverOk = false;
                            Connection = NULL;
                            break;
                        }
                        else //Все остальные случаи
                        {
                            return;
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                if (serverOk) //Если подключение успешно
                {
                    lg ->WriteMsg("Успешное подключение к серверу.");

                    //Отправляем на сервер логин
                    std::string request = "NEW_CLIENT::" + login.toStdString();
                    int requestSize = request.size();
                    send(Connection, (char*)&requestSize, sizeof(int), 0);
                    send(Connection, request.c_str(), requestSize, 0);

                    recv(Connection, (char*)&requestSize, sizeof(int), 0);  //Получаем от сервера размер ожидаемого сообщения
                    char* msgFromServer = new char[requestSize+1];          //Создаем чар нужного размера
                    msgFromServer[requestSize] = '\0';

                    //Получаем от сервера сообщение
                    if (recv(Connection, msgFromServer, requestSize, 0) > 0)
                    {
                        if (strcmp(msgFromServer, "CONNECT_SUCCESS") == 0)
                        {
                            //Если подключение к серверу успешное - получаем роль и идем в функцию SuccessLogin
                            query.exec("SELECT role FROM of_user_accounts WHERE login = '" + login + "'");
                            query.next();
                            role = query.value(0).toString();

                            if (!OfficeSystem::CheckPassword(passBd)) //Проверка безопасности пароля
                            {
                                QMessageBox msg;
                                msg.setText("Смените пароль на более надежный!");
                                msg.exec();
                            }

                            SuccessLogin(login, role); //Создание новых форм
                        }
                        else if (strcmp(msgFromServer, "LOGIN_ALREADY") == 0) //Если сервер говорит, что такой логин уже в сети
                        {
                            lg->WriteMsg("#L003 - Данный логин уже в сети."); //В журнал

                            popNotify->setPopupText("Данный логин уже в сети"); //Уведомление
                            popNotify->show(this);
                            return;
                        }
                    }
                }
                else //Вариант без сервера
                {
                    //Получаем роль и идем в функцию SuccessLogin
                    query.exec("SELECT role FROM of_user_accounts WHERE login = '" + login + "'");
                    query.next();
                    role = query.value(0).toString();

                    if (!OfficeSystem::CheckPassword(passBd))
                    {
                        QMessageBox msg;
                        msg.setText("Смените пароль на более надежный!");
                        msg.exec();
                    }

                    SuccessLogin(login, role);
                }

            }
            else //Если введен неверный пароль
            {
                lg->WriteMsg(login + ": попытка входа. Неверный пароль.");

                popNotify->setPopupText("Неверный пароль");
                popNotify->show(this);
                return;
            }
        }
        else //Если логина в базе нет
        {
            lg->WriteMsg(login + ": не зарегистрирован.");

            popNotify->setPopupText("Такой логин не зарегистрирован");
            popNotify->show(this);
            return;
        }
    }
}

int MainForm::ErrorConnectionServer(QString text_error) //Варианты исправления ошибки с подключением
{
    int res = 0;

    lg->WriteMsg(text_error); //Запишем ошибку в лог-файл

    QMessageBox msg;
    msg.setText(text_error);
    QPushButton *noServer = msg.addButton(("Продолжить без сервера"), QMessageBox::ActionRole);
    QPushButton *edit = msg.addButton(("Изменить данные подключения"), QMessageBox::ActionRole);
    QPushButton *exit = msg.addButton(("Закрыть программу"), QMessageBox::ActionRole);
    QPushButton *help = msg.addButton((" ? "), QMessageBox::ActionRole);

    msg.exec();

    if (msg.clickedButton() == noServer) //Без сервера
    {
        res = 2;
    }

    if (msg.clickedButton() == exit) //Выход
    {
        close();
    }

    if (msg.clickedButton() == edit) //Редактирование данных для подключения
    {
        EditSettingFile();
        res = 1;
    }

    if (msg.clickedButton() == help) //Справка
    {
        res = 3;
        QMessageBox help_msg;
        help_msg.setText("   Вы можете обратиться за помощью к администратору или решить проблему самостоятельно.\n\n   Нажмите кнопку 'Изменить данные подключения', если Вы знаете"
                         " текущий адрес сервера.\n\n Нажмите кнопку 'Продолжить без сервера', чтобы воспользоваться программой без сетевых функций.");
        help_msg.exec();
    }

    return res;
}

bool MainForm::CheckCapsLock() //Проверка капсЛока
{
    bool res = GetKeyState(VK_CAPITAL) &  0x0001;
    return res;
}


void MainForm::keyPressEvent(QKeyEvent *event)
{
    int key=event->key();
    if (key == 16777252) //Код клавиши CapsLock
    {
        if (CheckCapsLock())
        {
            ui->lb_caps->setText("   Включен Caps Lock");
        }
        else
        {
            ui->lb_caps->setText("");
        }
    }
    else
    {
        hkl = GetKeyboardLayout(threadId);
        code = LOWORD(hkl);

        if (code == 1033) //Английская раскладка
        {
            ui->lb_layout->setText("   EN");
        }
        else if (code == 1049) //Русская раскладка
        {
            ui->lb_layout->setText("   RU");
        }
        else
        {
            ui->lb_layout->setText("");
        }
    }
}

void MainForm::paintEvent(QPaintEvent *e) //Отрисовка фона
{
    if (bg.isNull())
    {
        QWidget::paintEvent(e);
        return;
    }

    QPainter p(this);

    //Масштабирование
    qreal xs = qreal(width()) / qreal(bg.width());
    qreal ys = qreal(height()) / qreal(bg.height());

    if (xs != 1 || ys != 1)
    {
        QImage img;
        int cx = qRound(e->rect().x() / xs);
        int cy = qRound(e->rect().y() / ys);
        int cw = qRound(e->rect().width() / xs);
        int ch = qRound(e->rect().height() / ys);
        cw = qMin(cw, bg.width() - cx);
        ch = qMin(ch, bg.height() - cy);
        img = bg.copy(cx, cy, cw, ch);
        cw = qRound(cw * xs);
        ch = qRound(ch * ys);
        img = img.scaled(cw, ch, Qt::IgnoreAspectRatio, Qt::FastTransformation);
        p.drawImage(e->rect().topLeft(), img);
    }
    else
    {
        p.drawImage(e->rect().topLeft(), bg, QRect(e->rect().x(), e->rect().y(),
                           qMin(e->rect().width(), bg.width()), qMin(e->rect().height(), bg.height())));
    }
}

void MainForm::on_bt_help_password_clicked()
{
    QMessageBox msg;
    QString question;                       //Вопрос
    QString login = ui->le_login->text();   //Логин
    QString answer_user;                    //Ответ пользователя
    QString answer_bd;                      //Ответ из базы данных
    QString new_password;                   //Новый пароль
    QSqlQuery query;                        //Запрос

    if (login != "") //Если поле Логина не пустое
    {
        lg->WriteMsg("Попытка восстановления пароля для пользователя " + login + "."); //Запишем информацию в лог-файл

        query.exec("SELECT login FROM of_user_accounts WHERE login = '" + login + "'"); //Проверка существования логина
        query.next();

        if (query.value(0).toString() == login) //Если логин существует
        {
            query.exec("SELECT question FROM of_user_accounts WHERE login = '" + login + "'"); //Делаем запрос к БД
            query.next();
            question = query.value(0).toString(); //Получаем вопрос для восстановления

            if (question == "") //Если вопроса нет
            {
                popNotify->setPopupText("Отсутствуют данные для восстановления пароль. Обратитесь к администратору");
                popNotify->show(this);
                return;
            }
        }
        else //Если логина не существует
        {
            popNotify->setPopupText("Такой логин не найден");
            popNotify->show(this);
            return;
        }
    }
    else //Если поле не заполнено
    {
        popNotify->setPopupText("Заполните поле имени пользователя и попробуйте еще раз");
        popNotify->show(this);
        return;
    }

    //Запрос ответа
    bool bOk;
    answer_user += QInputDialog::getText(0, "Введите ответ", question,
                                            QLineEdit::Normal, "", &bOk);
    if (!bOk) return;
    else
    {
        query.exec("SELECT answer FROM of_user_accounts WHERE login = '" + login + "'"); //Делаем запрос к БД
        if (query.next())
        {
            answer_bd = query.value(0).toString();
            if (answer_user == answer_bd) //Если ответ пользователя и ответ в базе совпал
            {
                new_password += QInputDialog::getText(0, "Введите новый пароль", "Пароль дольжен быть больше 8 символов, содержать буквы разного регистра и цифры",
                                                        QLineEdit::Password, "", &bOk);
                if (!bOk) return;
                else
                {
                    if (OfficeSystem::CheckPassword(new_password))
                    {
                            query.exec("UPDATE of_user_accounts SET password = '" + new_password + "' WHERE login = '" + login + "'"); //Меняем пароль

                            lg->WriteMsg("Успешная смена пароля для " + login + "."); //Запишем информацию в лог-файл

                            popNotify->setPopupText("Ваш пароль изменен");
                            popNotify->show(this);
                            return;
                    }
                    else
                    {
                        popNotify->setPopupText("Ваш пароль небезопасен");
                        popNotify->show(this);
                        return;
                    }
                }
            }
            else
            {
                popNotify->setPopupText("Ответ неверный");
                popNotify->show(this);
                return;
            }
        }
        else
        {
            popNotify->setPopupText("Отсутствуют данные для восстановления пароля. Обратитесь к администратору");
            popNotify->show(this);
            return;
        }
    }
}
