#include "userprofile.h"

UserProfile::UserProfile(QWidget *_parentForm, QWidget* _profile, QString _login)
{
    login = _login;                 //Логин пользователя
    profile = _profile;             //Форма для компоновки
    parentForm = _parentForm;       //Форма для отображения уведомлений

    popNotify = new PopNotify();    //Уведомления
    lg = new Log();                 //Журнализирование
    CreateComponents();             //Создание компонентов и размещение их на форме
}

void UserProfile::CreateComponents()
{
    lb_fio = new QLabel();
    lb_fio->setStyleSheet(OfficeSystem::GetStyleSheetLabel());

    lb_dept = new QLabel();
    lb_dept->setStyleSheet(OfficeSystem::GetStyleSheetLabel("black"));

    lb_role = new QLabel();
    lb_role->setStyleSheet(OfficeSystem::GetStyleSheetLabel("black"));

    lb_orgInfo = new QLabel();
    lb_orgInfo->setStyleSheet(OfficeSystem::GetStyleSheetLabel("black"));

    lb_question = new QLabel();
    lb_question->setStyleSheet(OfficeSystem::GetStyleSheetLabel("black"));

    GetData(); //Получение данных в выше созданые label

    //Поле ввода текущего пароля
    le_oldPass = new QLineEdit();
    le_oldPass->setGeometry(40, 140, 310, 50);
    le_oldPass->setMinimumSize(310, 50);
    le_oldPass->setPlaceholderText("Текущий пароль");
    le_oldPass->setEchoMode(QLineEdit::Password);
    le_oldPass->setStyleSheet(OfficeSystem::GetStyleSheetLineEdit()); //Получаем стиль из статического метода
    le_oldPass->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    //Поле ввода нового пароля
    le_newPass = new QLineEdit();
    le_newPass->setGeometry(40, 240, 370, 50);
    le_newPass->setMinimumSize(310, 50);
    le_newPass->setPlaceholderText("Новый пароль");
    le_newPass->setEchoMode(QLineEdit::Password);
    le_newPass->setStyleSheet(OfficeSystem::GetStyleSheetLineEdit());
    le_newPass->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    //Кнопка смены пароля
    bt_change = new QPushButton();
    bt_change->setGeometry(40, 40, 246, 45); //Х, Y, длина, высота
    bt_change->setMinimumSize(280, 50);
    bt_change->setText("Сменить пароль");
    bt_change->setStyleSheet(OfficeSystem::GetStyleSheetButton());
    bt_change->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(bt_change, SIGNAL(clicked()), this, SLOT(slotChangePassword()));

    //--------------------------------------------------- К О М П О Н О В К А ---------------------------------------------------//

    QVBoxLayout *vboxLeft = new QVBoxLayout(); //Компоновка левой части
    QVBoxLayout *vboxRight = new QVBoxLayout(); //Компоновка правой части

    QHBoxLayout *hboxGeneral = new QHBoxLayout(profile); //Общая компоновка

    //Отступы
    QSpacerItem *topSpacer = new QSpacerItem(1, 210, QSizePolicy::Fixed, QSizePolicy::Fixed);
    QSpacerItem *leftSpacer = new QSpacerItem(10, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    QSpacerItem *rightSpacer = new QSpacerItem(10, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);

    //1. Виджет для отображения информации о пользователе
    QVBoxLayout *infoUserGeneralLayout = new QVBoxLayout();

    QWidget *headerInfoUser = new QWidget(); //Создаем виджет
    headerInfoUser->setStyleSheet(OfficeSystem::GetStyleSheetWidget("top")); //Устанавливаем стиль для виджета
    headerInfoUser->setMaximumHeight(40);   //Установка максимальной высоты
    headerInfoUser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  //Политика размеров - заполнение по X и Y

    QHBoxLayout *headerInfoUserLayout = new QHBoxLayout(headerInfoUser); //Менеджер компоновки
    headerInfoUserLayout->addWidget(lb_fio, 0, Qt::AlignCenter);         //Добавляем в него label с ФИО
    infoUserGeneralLayout->addWidget(headerInfoUser);   //Помещяем в основной менеджер виджет

    QWidget *infoUser = new QWidget();
    infoUser->setMinimumSize(200, 150);
    infoUser->setStyleSheet(OfficeSystem::GetStyleSheetWidget("bottom"));
    infoUser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *infoUserLayout = new QVBoxLayout(infoUser);

    QLabel *tempDeptLabel = new QLabel("Подразделение: ");
    tempDeptLabel->setStyleSheet(OfficeSystem::GetStyleSheetLabel());
    QHBoxLayout *tempDeptLayout = new QHBoxLayout();
    tempDeptLayout->addWidget(tempDeptLabel, 0, Qt::AlignLeft);
    tempDeptLayout->addWidget(lb_dept, 1, Qt::AlignLeft);
    infoUserLayout->addLayout(tempDeptLayout); //Add

    QLabel *tempRoleLabel = new QLabel("Вид доступа: ");
    tempRoleLabel->setStyleSheet(OfficeSystem::GetStyleSheetLabel());
    QHBoxLayout *tempRoleLayout = new QHBoxLayout();
    tempRoleLayout->addWidget(tempRoleLabel, 0, Qt::AlignLeft);
    tempRoleLayout->addWidget(lb_role, 1, Qt::AlignLeft);
    infoUserLayout->addLayout(tempRoleLayout); //Add

    QLabel *tempQuestionLabel = new QLabel("Вопрос для восстановления пароля: ");
    tempQuestionLabel->setStyleSheet(OfficeSystem::GetStyleSheetLabel());
    QHBoxLayout *tempQuestionLayout = new QHBoxLayout();
    tempQuestionLayout->addWidget(tempQuestionLabel, 0, Qt::AlignLeft);
    tempQuestionLayout->addWidget(lb_question, 1, Qt::AlignLeft);
    infoUserLayout->addLayout(tempQuestionLayout); //Add

    infoUserGeneralLayout->addWidget(infoUser);

    //2. Информация об организации
    QVBoxLayout *infoOrgGeneralLayout = new QVBoxLayout();
    //infoOrgGeneralLayout->addSpacerItem(topSpacer);
    QWidget *headerOrgInfo = new QWidget();
    headerOrgInfo->setStyleSheet(OfficeSystem::GetStyleSheetWidget("top"));
    headerOrgInfo->setMaximumHeight(40);
    headerOrgInfo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout *headerInfoOrgLayout = new QHBoxLayout(headerOrgInfo);
    QLabel *headerOrgLabel = new QLabel("Сведения об организации");
    headerOrgLabel->setStyleSheet(OfficeSystem::GetStyleSheetLabel());
    headerInfoOrgLayout->addWidget(headerOrgLabel, 0, Qt::AlignCenter);
    infoOrgGeneralLayout->addWidget(headerOrgInfo);

    orgInfo = new QWidget();
    orgInfo->setMinimumSize(200, 150);
    orgInfo->setStyleSheet(OfficeSystem::GetStyleSheetWidget("bottom"));
    orgInfo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *orgInfoLayout = new QVBoxLayout(orgInfo);
    orgInfoLayout->addWidget(lb_orgInfo);

    infoOrgGeneralLayout->addWidget(orgInfo);

    vboxRight->addSpacerItem(topSpacer);
    vboxRight->addLayout(infoUserGeneralLayout);
    vboxRight->addSpacing(60);
    vboxRight->addLayout(infoOrgGeneralLayout);

    QSpacerItem *bottomExpSpacer = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding); //Отступ с заполнением всего пространства

    if (lb_role->text() == "Администратор")
    {
        bt_info = new QPushButton();
        bt_info->setGeometry(40, 40, 246, 45);
        bt_info->setMinimumSize(280, 50);
        bt_info->setText("Изменить информацию");
        bt_info->setStyleSheet(OfficeSystem::GetStyleSheetButton());  //Получаем стиль из статического метода класса OfficeSystem
        bt_info->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        connect(bt_info, SIGNAL(clicked()), this, SLOT(slotNewInfoOrg()));

        vboxRight->addSpacing(25);
        vboxRight->addWidget(bt_info);
        vboxRight->addSpacerItem(bottomExpSpacer);
    }
    else
        vboxRight->addSpacerItem(bottomExpSpacer);


    QSpacerItem *fixSpacer = new QSpacerItem(1, 260, QSizePolicy::Fixed, QSizePolicy::Fixed);

    vboxLeft->addSpacerItem(fixSpacer);
    vboxLeft->addWidget(le_oldPass, 0, Qt::AlignLeft);
    vboxLeft->addSpacing(20);
    vboxLeft->addWidget(le_newPass, 1, Qt::AlignLeft);
    vboxLeft->addSpacing(20);
    vboxLeft->addWidget(bt_change, 2, Qt::AlignLeft);
    vboxLeft->addSpacing(60);

    le_question = new QLineEdit();
    le_question->setStyleSheet(OfficeSystem::GetStyleSheetLineEdit());
    le_question->setGeometry(40, 240, 370, 50);
    le_question->setMinimumSize(310, 50);
    le_question->setPlaceholderText("Вопрос");
    le_question->setStyleSheet(OfficeSystem::GetStyleSheetLineEdit()); //Получаем стиль
    le_question->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    le_answer = new QLineEdit();
    le_answer->setStyleSheet(OfficeSystem::GetStyleSheetLineEdit());
    le_answer->setGeometry(40, 240, 370, 50);
    le_answer->setMinimumSize(310, 50);
    le_answer->setPlaceholderText("Ответ");
    le_answer->setStyleSheet(OfficeSystem::GetStyleSheetLineEdit()); //Получаем стиль
    le_answer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    bt_setQuestion = new QPushButton();
    bt_setQuestion->setGeometry(40, 40, 246, 45);
    bt_setQuestion->setMinimumSize(280, 50);

    //Текст для кнопки
    if (lb_question->text() == "Задан")
    {
        bt_setQuestion->setText("Изменить вопрос");
    }
    else
    {
        bt_setQuestion->setText("Добавить вопрос");
    }

    bt_setQuestion->setStyleSheet(OfficeSystem::GetStyleSheetButton());  //Получаем стиль
    bt_setQuestion->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(bt_setQuestion, SIGNAL(clicked()), this, SLOT(slotSetQuestion()));

    vboxLeft->addWidget(le_question, 0, Qt::AlignLeft);
    vboxLeft->addSpacing(20);
    vboxLeft->addWidget(le_answer, 1, Qt::AlignLeft);
    vboxLeft->addSpacing(20);
    vboxLeft->addWidget(bt_setQuestion, 2, Qt::AlignLeft);
    vboxLeft->addSpacing(50);

    vboxLeft->addSpacerItem(bottomExpSpacer);

    hboxGeneral->addSpacerItem(leftSpacer);
    hboxGeneral->addLayout(vboxLeft);
    hboxGeneral->addSpacing(30);
    hboxGeneral->addLayout(vboxRight);
    hboxGeneral->addSpacerItem(rightSpacer);
}

void UserProfile::GetData()
{
    QSqlQuery query;

    //Проверяем существование таблицы (возвращает bool)
    query.exec("SELECT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name = 'of_org_info')");
    query.next();

    if(!(query.value(0).toBool())) //Если не существует - создаем
    {
        query.exec("CREATE TABLE IF NOT EXISTS of_org_info (org_info text, PRIMARY KEY(org_info))");
    }
    else //Если существует - получаем информацио об организации
    {
        query.exec("SELECT org_info FROM of_org_info");
        if  (query.next())
        {
            lb_orgInfo->setText(query.value(0).toString());
        }
    }

    //Запрашиваем ФИО, вод доступа, подразделение
    query.exec("SELECT fio, role, dept FROM of_user_accounts WHERE login = '" + login + "'");
    if (query.next())
    {
        lb_fio->setText(query.value(0).toString());
        lb_role->setText(query.value(1).toString());
        lb_dept->setText(query.value(2).toString());
    }

    //Проверяем наличие информации для восстановления пароля
    query.exec("SELECT question, answer FROM of_user_accounts WHERE login = '" + login + "'");
    if (query.next())
    {
        if (query.value(0).toString() != "" && query.value(1).toString() != "")
        {
            lb_question->setText("Задан");
        }
        else
        {
            lb_question->setText("Не задан");
        }
    }
}

void UserProfile::slotChangePassword()
{
    QString oldPass = le_oldPass->text();
    QString newPass = le_newPass->text();
    QSqlQuery query;

    if (oldPass != "" && newPass != "") //Если поля заполнены
    {
        query.exec("SELECT password FROM of_user_accounts WHERE login = '" + login + "'"); //Получаем текущий пароль
        if (query.next())
        {
            QString passBd = query.value(0).toString();

            if (passBd == oldPass) //Если текущие пароли совпадают
            {
                if (OfficeSystem::CheckPassword(newPass)) //Проверяем безопасность
                {
                    if (query.exec("UPDATE of_user_accounts SET password = '" + newPass + "' WHERE login = '" + login + "'")) //Меняем пароль
                    {
                        lg->WriteMsg(login + ": пароль изменен.");
                        le_oldPass->clear();
                        le_newPass->clear();
                        popNotify->setPopupText("Пароль изменен");
                        popNotify->show(parentForm);
                    }
                    else
                    {
                        popNotify->setPopupText("Ошибка при сбросе пароля. Попробуйте еще раз");
                        popNotify->show(parentForm);
                    }
                }
                else
                {
                    popNotify->setPopupText("Новый пароль недостаточно надежен");
                    popNotify->show(parentForm);
                }
            }
            else
            {
                popNotify->setPopupText("Неверно указан текущий пароль");
                popNotify->show(parentForm);
            }
        }
        else
        {
            popNotify->setPopupText("База данных повреждена. Обратитесь к администратору.");
            popNotify->show(parentForm);
        }
    }
    else
    {
        popNotify->setPopupText("Заполните поля");
        popNotify->show(parentForm);
    }
}

void UserProfile::slotSetQuestion()
{
    QSqlQuery query;
    QString pass, passBd;

    if (le_question->text() != "" && le_answer->text() != "") //Если поля не пустые
    {
        //Запрашиваем пароль
        bool ok;
        pass = QInputDialog::getText(0, "Секретный вопрос", "Введите пароль:",
                                     QLineEdit::Password, "", &ok);
        if (!ok)
        {
            return;
        }
        else
        {
            query.exec("SELECT password FROM of_user_accounts WHERE login = '" + login + "'");

            if (query.next())
            {
                passBd = query.value(0).toString();

                if (pass == passBd) //Проверяем пароли
                {
                    //Меняем информацию в базе
                    query.exec("UPDATE of_user_accounts SET question = '" + le_question->text() + "', answer = '" + le_answer->text() + "' WHERE login = '" + login + "'"); //Делаем запрос к БД
                    lb_question->setText("Задан");
                    le_answer->clear();
                    le_question->clear();

                    lg->WriteMsg(login + ": изменен вопрос для восстановления пароля.");
                    bt_setQuestion->setText("Изменить вопрос");
                    popNotify->setPopupText("Данные сохранены");
                    popNotify->show(parentForm);
                }
                else
                {
                    popNotify->setPopupText("Неверный пароль");
                    popNotify->show(parentForm);
                }
            }
            else
            {
                popNotify->setPopupText("Ошибка. Попробуйте позже");
                popNotify->show(parentForm);
            }
        }
    }
    else
    {
        popNotify->setPopupText("Заполните поля");
        popNotify->show(parentForm);
    }
}

void UserProfile::slotNewInfoOrg()
{
    //QByteArray t = "100";
    //emit sendMsg(t);

    dialog = new QDialog(); //Создаем диалог
    dialog->setStyleSheet(OfficeSystem::GetStyleSheetDialog()); //Стиль
    dialog->setGeometry(400, 400, 500, 500);    //Метоположение и размеры

    //Отступы
    QSpacerItem *vertSpaser = new QSpacerItem(1, 15, QSizePolicy::Fixed, QSizePolicy::Fixed);
    QSpacerItem *horzSpaser = new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);

    te_newInfo = new QTextEdit();
    te_newInfo->setMinimumSize(300, 300);
    te_newInfo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    te_newInfo->setText(lb_orgInfo->text());
    te_newInfo->setStyleSheet(OfficeSystem::GetStyleSheetTextEdit());

    QPushButton *bt = new QPushButton();
    bt->setMinimumSize(246, 25);
    bt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    bt->setText("Применить");
    connect(bt, SIGNAL(clicked()), this, SLOT(slotSetNewInfoOrg())); //Указываем, что делать при клике (перейти в slotSetNewInfoOrg)

    QVBoxLayout *vertLayout = new QVBoxLayout();
    vertLayout->addSpacerItem(vertSpaser);
    vertLayout->addWidget(te_newInfo);
    vertLayout->addSpacing(25);
    vertLayout->addWidget(bt);
    vertLayout->addSpacerItem(vertSpaser);

    QHBoxLayout *horzLayout = new QHBoxLayout(dialog);
    horzLayout->addSpacerItem(horzSpaser);
    horzLayout->addLayout(vertLayout);
    horzLayout->addSpacerItem(horzSpaser);

    dialog->show();
}

void UserProfile::slotSetNewInfoOrg()
{
    QSqlQuery query;
    QString newInfo = te_newInfo->toPlainText();

    if (newInfo != "")
    {
        query.exec("UPDATE of_org_info SET org_info = '" + newInfo + "'"); //Обновляем информацию в базе
        dialog->hide(); //Закрываем диалог

        lb_orgInfo->setText(newInfo);   //Записываем новую информацию в label
        popNotify->setPopupText("Информация обновлена");    //Уведомление
        popNotify->show(parentForm);
    }
    else
    {
        popNotify->setPopupText("Заполните поле");
        popNotify->show(parentForm);
    }
}
