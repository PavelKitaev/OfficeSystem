#include "safety.h"


Safety::Safety(SOCKET _connected, QWidget *_safety, QWidget *_pupNotify_form, QString _login)
{
    connectedSocket = _connected;               //Соединение с сервером
    safety = _safety;                           //Виджет
    pupNotifyForm = _pupNotify_form;            //Виджет для уведомлений
    login = _login;                             //Логин пользователя
    role = GetRole();                           //Роль пользователя

    RegServer();                                //Сообщаем серверу, что данный логин будет принимать данные с микр.контр.

    safetyOk = false;                           //Режим охраны

    popNotify = new PopNotify();                //Уведомления

    CheckTable();                               //Проверка существования таблицы и ее создание, в случае отсутствия
    GetDataTable();                             //Получение информации из таблицы
    CreateComponents();                         //Создание и компоновка объектов

    GetSafetyData();

    timer = new QTimer(this);       //Таймер для обновления пользователей раз в 1 минуту
    connect(timer, SIGNAL(timeout()), this, SLOT(GetSafetyData()));
    timer->start(5000);                         //Обновление списка пользователей онлайн раз в минуту
}

void Safety::receive(QByteArray const& str)
{
    QString data = str;
    QMap<QString, QString> parsedData;

    int l = data.length();

    QString first;
    QString second;
    int i = 0;
    while (data[i] != '\0')
    {
        if (data[i] == ':')
        {
            i++;
            while (data[i] != ';')
            {
               second += data[i];
               i++;
            }

            if (i < l-1) //Исключаем предупреждение о выходе за границы на последней итерации цикла
            {
                i++;
            }

            parsedData.insert(first, second);
            first.clear();
            second.clear();
            continue;
        }
        first += data[i];
        if (i < l-1) //Исключаем предупреждение о выходе за границы на последней итерации цикла
        {
            i++;
        }
        else
        {
            break;
        }
    }

    QMap<QString, QString> res;
    QMapIterator<QString, QString> it(dataTable);
    QString temp_left, temp_right;

    while(it.hasNext())
    {
        it.next();
        QMapIterator<QString, QString> it_data(parsedData);
        QString t = it.key();
        bool ok = false;
        while(it_data.hasNext())
        {
            it_data.next();
            if (it_data.key() == t)
            {
                temp_right = it_data.value();
                ok = true;
                break;
            }
        }

        if (ok)
        {
            temp_left = it.value();
            res.insert(temp_left, temp_right);
        }
    }

    int iq = 0;
    int j = 0;
    QMapIterator<QString, QString> it_res(res);

    while(it_res.hasNext())
    {
        it_res.next();
        if (it_res.value() == "0")
        {
            lbs[iq]->setText(it_res.key() + " - " + "Открыто");
        }
        else
        {
            lbs[iq]->setText(it_res.key() + " - " + "Закрыто");
        }

        if (it_res.value() == "0") //Если дверь открыта
        {
            if (safetyOk) //И включена охрана
            {
                //Новый стить
                lbs[iq]->setStyleSheet("QLabel { background: rgb(234, 95, 1); border-style: solid; border-width: 2px; border-radius: 15px; border-color: rgb(76, 155, 235); font: Lato; font-size: 18px;}");

                if (j == 0)
                {
                    player->play(); //Включаем звук
                }
                j++;
            }
            else
            {
                lbs[iq]->setStyleSheet("QLabel{ background: none; border-style: solid; border-width: 2px; border-radius: 15px; border-color: rgb(76, 155, 235); font: Lato; font-size: 18px;}");
            }

        }
        else
        {
            lbs[iq]->setStyleSheet("QLabel{ background: none; border-style: solid; border-width: 2px; border-radius: 15px; border-color: rgb(76, 155, 235); font: Lato; font-size: 18px; }");
        }

        if (j != 0) //Меняем стиль шапки, если охрана включена и есть соответствующий сигнал с сенсора
        {
            header->setStyleSheet(OfficeSystem::GetStyleSheetWidget("top", "rgb(234, 95, 1)"));
            headerName->setStyleSheet(OfficeSystem::GetStyleSheetLabel("black"));
        }
        else
        {
            header->setStyleSheet(OfficeSystem::GetStyleSheetWidget("top"));
            headerName->setStyleSheet(OfficeSystem::GetStyleSheetLabel());
        }
        iq++;
    }
}

Safety::~Safety()
{

}

QString Safety::GetRole()
{
    //Запрос к базе
    QString res;
    query.exec("SELECT role FROM of_user_accounts WHERE login = '" + login + "'");

    if (query.next())
    {
        res = query.value(0).toString();
    }
    else
    {
        res = "none";
    }
    return res;
}

void Safety::CreateComponents()
{
    player = new QMediaPlayer;                          //Плеер
    player->setMedia(QUrl("qrc:/image/alarm.wav"));     //Звук
    hboxGeneral = new QHBoxLayout(safety);

    //Взять под охрану
    bt_safety = new QPushButton();
    bt_safety->setGeometry(800, 10, 200, 40);
    bt_safety->setText("Взять под охрану");
    bt_safety->setMinimumSize(246, 45);
    bt_safety->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    bt_safety->setStyleSheet(OfficeSystem::GetStyleSheetButton());
    connect(bt_safety, SIGNAL(clicked()), this, SLOT(slotSafetyButton()));

    if (role == "Администратор") //Если роль Адм, создаем доп. компоненты для изменения данных
    {
        idSensor = new QLineEdit();
        idSensor->setMinimumSize(246, 45);
        idSensor->setStyleSheet(OfficeSystem::GetStyleSheetLineEdit());
        idSensor->setPlaceholderText("Идентификатор сенсора");
        idSensor->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        sensorName = new QLineEdit();
        sensorName->setMinimumSize(246, 45);
        sensorName->setStyleSheet(OfficeSystem::GetStyleSheetLineEdit());
        sensorName->setPlaceholderText("Имя отображения");
        sensorName->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        insert = new QPushButton();
        insert->setMinimumSize(246, 45);
        insert->setStyleSheet(OfficeSystem::GetStyleSheetButton());
        insert->setText("Добавить");
        insert->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        connect(insert, SIGNAL(clicked()), this, SLOT(slotInsertDataInTable()));

        tableView = new QTableView(safety);
        tableView->setGeometry(800, 100, 400, 400);
        tableView->setMinimumSize(550, 250);
        tableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        tableView->show();

        modelDevice = new QSqlTableModel(this);
        modelDevice->setTable(tableName);
        modelDevice->select();
        tableView->setModel(modelDevice);
        tableView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));

        //Устанавливаем нужную ширину на каждый столбец
        tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch); //ширина 1-3 столбцов по содержимому
        tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        tableView->horizontalHeader()->setStyleSheet(OfficeSystem::GetStyleSheetHorizontalHeaderView());
        tableView->verticalHeader()->setStyleSheet(OfficeSystem::GetStyleSheetVerticalHeaderView());
        tableView->setStyleSheet(OfficeSystem::GetStyleSheetTableView());

        modelDevice->setHeaderData(0, Qt::Horizontal, "Идентификатор сенсора");
        modelDevice->setHeaderData(1, Qt::Horizontal, "Имя отображения");
    }

    //-------------------------------------------- К О М П О Н О В К А --------------------------------------------//

    //Отступы
    QSpacerItem *topSpacer = new QSpacerItem(1, 210, QSizePolicy::Fixed, QSizePolicy::Fixed);
    QSpacerItem *rightSpacer = new QSpacerItem(241, 1, QSizePolicy::Minimum, QSizePolicy::Minimum);
    QSpacerItem *leftSpacer = new QSpacerItem(20, 1, QSizePolicy::Maximum, QSizePolicy::Maximum);
    QSpacerItem *bottomSpacer = new QSpacerItem(30, 1, QSizePolicy::Fixed, QSizePolicy::Fixed);

    //Менеджеры
    QVBoxLayout *vboxSensors = new QVBoxLayout();
    QVBoxLayout *vboxRole = new QVBoxLayout();

    //1. Шапка
    header = new QWidget(safety); //Создаем виджет
    header->setStyleSheet(OfficeSystem::GetStyleSheetWidget("top")); //Задаем стиль
    header->setMinimumSize(300, 40); //Задаем максимальные размеры
    header->setMaximumSize(700, 40); //Задаем минимальные размеры
    header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    headerName = new QLabel();
    headerName->setText("Показания датчиков");
    headerName->setGeometry(50, 800, 500, 60);
    headerName->setStyleSheet(OfficeSystem::GetStyleSheetLabel());
    QHBoxLayout *header_layout = new QHBoxLayout(header); //Менеджер компоновки для шапки
    header_layout->addWidget(headerName, 0, Qt::AlignCenter); //Помещаем в шапку label с именем собеседника

    vboxSensors->addSpacerItem(topSpacer);
    vboxSensors->addWidget(header);

   //2. Поле вывода информации
   QWidget *sensorsWidget = new QWidget(safety);
   sensorsWidget->setStyleSheet(OfficeSystem::GetStyleSheetWidget("bottom"));
   sensorsWidget->setMaximumSize(700, 700);
   sensorsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

   vboxLabel = new QVBoxLayout(sensorsWidget);

   QScrollArea * scrollArea = new QScrollArea(sensorsWidget);
   QScrollBar *vert_scrollbar = new QScrollBar();
   vert_scrollbar->setStyleSheet(OfficeSystem::GetStyleSheetVerticalScrollBar());
   QScrollBar *horz_scrollbar = new QScrollBar();
   horz_scrollbar->setStyleSheet(OfficeSystem::GetStyleSheetHorizontalScrollBar());

   scrollArea->setVerticalScrollBar(vert_scrollbar);
   scrollArea->setHorizontalScrollBar(horz_scrollbar);
   scrollArea->setGeometry(0, 0, 500, 500);
   scrollArea->setWidgetResizable( true );
   vboxLabel->addWidget( scrollArea );

   QWidget * container = new QWidget();
   scrollArea->setWidget( container );
   vboxLabel = new QVBoxLayout(container);

   for (int i = 0; i < lbs.size(); i++)
   {
       vboxLabel->addWidget(lbs[i]);
       vboxLabel->addSpacing(15);
   }

   vboxSensors->addWidget(sensorsWidget);
   vboxSensors->addSpacerItem(bottomSpacer);

   hboxGeneral->addSpacerItem(leftSpacer);

   QVBoxLayout *bt_safetyLayout = new QVBoxLayout();
   bt_safetyLayout->addSpacerItem(topSpacer);
   bt_safetyLayout->addSpacing(45);
   bt_safetyLayout->addWidget(bt_safety, 0, Qt::AlignTop);

   hboxGeneral->addLayout(bt_safetyLayout);
   hboxGeneral->addSpacing(25);
   hboxGeneral->addLayout(vboxSensors);

   //3. Таблица для администратора
   if (role == "Администратор")
   {
       QVBoxLayout *buttonTable = new QVBoxLayout();

       QSpacerItem *bottomButtomSpacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);

       buttonTable->addSpacerItem(topSpacer);
       buttonTable->addSpacing(45);
       buttonTable->addWidget(idSensor);
       buttonTable->addSpacing(25);
       buttonTable->addWidget(sensorName);
       buttonTable->addSpacing(25);
       buttonTable->addWidget(insert);
       buttonTable->addSpacerItem(bottomButtomSpacer);

       vboxRole->addSpacerItem(topSpacer);
       vboxRole->addWidget(tableView);
       vboxRole->addSpacerItem(bottomSpacer);

       hboxGeneral->addSpacing(25);
       hboxGeneral->addLayout(buttonTable);
       hboxGeneral->addSpacing(25);
       hboxGeneral->addLayout(vboxRole);
   }

   hboxGeneral->addSpacerItem(rightSpacer);
}

void Safety::GetDataTable()
{
    query.exec("SELECT * FROM " + tableName); //Получаем всю таблицу

    while(query.next())
    {
        dataTable.insert(query.value(0).toString(), query.value(1).toString());
    }

    QMapIterator<QString, QString> it(dataTable); //Создаем итератор для прохода по принятым данным из таблицы

    while(it.hasNext()) //Создаем виджеты
    {
        it.next();
        QLabel *t = new QLabel(safety);
        t->setAlignment(Qt::AlignCenter);
        //t->setMinimumSize(300, 50);
        lbs.push_back(t);
    }
}

void Safety::CheckTable()
{
    query.exec("SELECT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name = '" + tableName + "')");
    if (query.next())
    {
        if (!query.value(0).toBool())
        {
            query.exec("CREATE TABLE IF NOT EXISTS " + tableName + " (id_sensor text NOT NULL, sensor_name text, PRIMARY KEY(id_sensor))");
        }
    }
}

void Safety::RegServer()
{
    QByteArray r = ("SECURITY_CLIENT::" + login).toLocal8Bit();

    const char* request = r.data();
    int request_size = strlen(request)+1;

    //Отправляем запрос на сервер
    send(connectedSocket, (char*)&request_size, sizeof(int), 0);
    send(connectedSocket, request, request_size, 0);
}

void Safety::slotInsertDataInTable() //Добавление данных в таблицу
{
    if (idSensor->text() != "" && sensorName->text() != "")
    {
        query.exec("INSERT INTO " + tableName + " (id_sensor, sensor_name) VALUES ('" + idSensor->text() + "', '" + sensorName->text() + "')");
        modelDevice->select();

        idSensor->clear();
        sensorName->clear();
        popNotify->setPopupText("Таблица обновлена.\nПерезагрузите программу");
        popNotify->show(pupNotifyForm);

    }
    else
    {
        popNotify->setPopupText("Заполните поля");
        popNotify->show(pupNotifyForm);
    }
}

void Safety::slotCustomMenuRequested(QPoint pos) //Меню удаления записи из таблицы (для адм)
{
    QMenu * menu = new QMenu(safety);
    QAction * deleteDevice = new QAction(("Удалить запись из таблицы"), this);
    connect(deleteDevice, SIGNAL(triggered()), this, SLOT(slotRemoveRecord())); // Обработчик удаления записи
    menu->addAction(deleteDevice);
    menu->popup(tableView->viewport()->mapToGlobal(pos));
}

void Safety::slotRemoveRecord()
{
    //Выясняем, какая из строк была выбрана
    int row = tableView->selectionModel()->currentIndex().row();
    if(row >= 0)
    {
        if (QMessageBox::warning(safety, ("Удаление записи"), ("Вы уверены, что хотите удалить эту запись?"),
                                 QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        {
            //При отрицательном ответе делаем откат действий и закрываем диалог без удаления записи
            QSqlDatabase::database().rollback();
            return;
        }
        else
        {
            //В противном случае производим удаление записи.
            if(!modelDevice->removeRow(row))
            {
                QMessageBox::warning(safety, ("Уведомление"),
                                     ("Не удалось удалить запись\n"
                                      "Возможно она используется другими таблицами\n"));
            }
            modelDevice->select();
        }
    }
}

void Safety::slotSafetyButton()
{
    if (!safetyOk)                                      //Если охрана выключена
    {
        timer->setInterval(1000);

        bt_safety->setText("Снять охрану");             //Меняем текст в кнопке

        popNotify->setPopupText("Взято под охрану");
        popNotify->show(pupNotifyForm);

        safetyOk = true;                                //Включаем
    }
    else if (safetyOk)                                  //Если охрана включена
    {
        timer->setInterval(5000);

        popNotify->setPopupText("Снято с охраны");
        popNotify->show(pupNotifyForm);

        bt_safety->setText("Взять под охрану");
        safetyOk = false;                               //Выключаем
    }
}

void Safety::GetSafetyData()
{
    const char* request = "GET_SENSORS_DATA::";
    int request_size = strlen(request)+1;

    //Отправляем запрос на сервер
    send(connectedSocket, (char*)&request_size, sizeof(int), 0);
    send(connectedSocket, request, request_size, 0);
}
