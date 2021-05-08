#include "notes.h"

Notes::Notes(QWidget *_parent_form, QWidget *_notes, QString _login)
{
    notes = _notes;                                 //Виджет, куда будут отрисованы элементы заметок
    parentForm = _parent_form;                      //Виджет, где будут отображаться уведомления
    login = _login;                                 //Логин текущего пользователя
    tabName = GetTabName();                         //Имя таблицы из базы данных

    popNotify = new PopNotify();                    //Уведомления
    modelDevice = new QSqlTableModel(parentForm);   //Таблица

    CreateComponents();                             //Размещение компонентов
}

Notes::~Notes()
{

}

void Notes::CreateComponents()
{
    vbox = new QVBoxLayout(notes); //Менеджер компоновки

    //Скроллбары
    QScrollBar *scrollbar_note = new QScrollBar();
    scrollbar_note->setStyleSheet(OfficeSystem::GetStyleSheetVerticalScrollBar());

    QScrollBar *scrollbar_text = new QScrollBar();
    scrollbar_text->setStyleSheet(OfficeSystem::GetStyleSheetVerticalScrollBar());

    QScrollBar *scrollbar_term = new QScrollBar();
    scrollbar_term->setStyleSheet(OfficeSystem::GetStyleSheetVerticalScrollBar());

    //Иконка кнопки добавления заметки
    QPixmap pixmap(":/image/add-note.png");
    QIcon ButtonIcon(pixmap);

    //Кнопка добавления записи
    bt_enter = new QPushButton(notes);
    bt_enter->setGeometry(1350, 700, 75, 75); //Х, Y, длина, высота
    bt_enter->setIcon(ButtonIcon);
    bt_enter->setIconSize(pixmap.rect().size());
    bt_enter->setStyleSheet(OfficeSystem::GetStyleSheetButtonForIcon());
    connect(bt_enter, SIGNAL(clicked()), this, SLOT(slotEnterButton()));

    //Отображение текущей даты
    lb_date = new QLabel(notes);
    lb_date->setGeometry(10, 10, 200, 60);
    lb_date->setStyleSheet(OfficeSystem::GetStyleSheetLabel());
    lb_date->setText("Сегодня " + QDate::currentDate().toString(Qt::SystemLocaleLongDate));

    //Текст заметки
    te_text = new QTextEdit(notes);
    te_text->setGeometry(20, 800, 500, 60);
    te_text->setMaximumSize(1200, 100);
    te_text->setVerticalScrollBar(scrollbar_text);
    te_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    te_text->setStyleSheet(OfficeSystem::GetStyleSheetTextEdit());

    //Текст "срока"
    te_term = new QTextEdit(notes);
    te_term->setGeometry(560, 800, 200, 60);
    te_term->setMaximumSize(500, 100);
    te_term->setVerticalScrollBar(scrollbar_term);
    te_term->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    te_term->setStyleSheet(OfficeSystem::GetStyleSheetTextEdit());

    //Создание таблицы
    modelDevice->setTable(tabName);
    modelDevice->select();
    modelDevice->setHeaderData(1, Qt::Horizontal, "Содержание");
    modelDevice->setHeaderData(2, Qt::Horizontal, "Срок");
    modelDevice->setHeaderData(3, Qt::Horizontal, "Дата создания записи");

    tv_table = new QTableView(notes);
    tv_table->setGeometry(20, 20, 200, 200);
    tv_table->setModel(modelDevice);
    tv_table->setVerticalScrollBar(scrollbar_note);
    tv_table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tv_table->horizontalHeader()->setStyleSheet(OfficeSystem::GetStyleSheetHorizontalHeaderView());

    tv_table->verticalHeader()->setStyleSheet(OfficeSystem::GetStyleSheetVerticalHeaderView());
    tv_table->setStyleSheet(OfficeSystem::GetStyleSheetTableView());
    tv_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tv_table, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));

    tv_table->hideColumn(0); //Скрываем столбец id_note (0)
    tv_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); //Растягиваем столбец на всю длину
    tv_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents); //Ширина по содержимому
    tv_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents); //Ширина по содержимому


    //---------------------------------------------------------------- К О М П О Н О В К А ----------------------------------------------------------------//

    //Отступы
    QSpacerItem *top_spacer = new QSpacerItem(1, 182, QSizePolicy::Fixed, QSizePolicy::Fixed); //Отступ от верхнего края окна
    QSpacerItem *bottom_spacer = new QSpacerItem(1, 30, QSizePolicy::Fixed, QSizePolicy::Fixed); //Отступ от нижнего края окна
    QSpacerItem *side_spacer = new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Fixed); //Отступ от нижнего края окна)

    //Менеджер для даты
    QSpacerItem *date_left_spacer = new QSpacerItem(5000, 1, QSizePolicy::Expanding, QSizePolicy::Fixed); //Отступ от нижнего края окна)
    QHBoxLayout *date = new QHBoxLayout();
    date->addSpacerItem(date_left_spacer);
    date->addWidget(lb_date);
    date->addSpacerItem(side_spacer);

    //Менеджер для полей ввода и кнопки
    QHBoxLayout *fields_hbox = new QHBoxLayout();
    fields_hbox->addSpacerItem(side_spacer);
    fields_hbox->addWidget(te_text);
    fields_hbox->addSpacing(10);
    fields_hbox->addWidget(te_term);
    fields_hbox->addWidget(bt_enter);
    fields_hbox->addSpacerItem(side_spacer);

    //Менеджер для таблицы
    QHBoxLayout *central_hbox = new QHBoxLayout();
    central_hbox->addSpacerItem(side_spacer);
    central_hbox->addWidget(tv_table);
    central_hbox->addSpacerItem(side_spacer);

    //Добавляем все в основной менеджер
    vbox->addSpacerItem(top_spacer);
    vbox->addLayout(date);
    vbox->addLayout(central_hbox);
    vbox->addSpacing(25);
    vbox->addLayout(fields_hbox);
    vbox->addSpacerItem(bottom_spacer);
}

void Notes::RefreshTable() //Обновление таблицы
{
    modelDevice->select();
    popNotify->setPopupText("Таблица обновлена");
    popNotify->show(parentForm);
}

QString Notes::GetTabName()
{
    QString name = "note_" + login;
    QString res = "-1";
    bool control = false;

    //Проверяем существование таблицы of_user_notes с названиями таблиц заметок пользователей
    query.exec("SELECT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name = 'of_user_notes')");
    query.next();

    if (!(query.value(0).toBool())) //Если таблицы не существует - создаем, добавляем запись. и создаем таблицу для заметок пользователя
    {
        query.exec("CREATE TABLE IF NOT EXISTS of_user_notes (idn bigserial NOT NULL,\
                   login character varying(30),\
                   tabl_name character varying(40),\
                   PRIMARY KEY (idn),\
                   CONSTRAINT fk_notes\
                   FOREIGN KEY (login) REFERENCES of_user_accounts(login)\
                   ON UPDATE CASCADE\
                   ON DELETE CASCADE)");


        query.exec("INSERT INTO of_user_notes VALUES ('" + login + "', '" + name.toLower() + "')");
        query.exec("CREATE TABLE " + name.toLower() + " (id_note bigserial NOT NULL, text_notes text, term text, date_added character varying(16), PRIMARY KEY(id_note))");
        res = name.toLower();
    }
    else //Если существует - получаем название таблицы с заметками пользователя
    {
        query.exec("SELECT tabl_name FROM of_user_notes WHERE login = '" + login + "'");
        query.next();

        if (query.value(0).toString() == "") //Если записи не существует - создаем запись и таблицу
        {
            query.exec("INSERT INTO of_user_notes VALUES ('" + login + "', '" + name.toLower() + "')");
            query.exec("CREATE TABLE " + name.toLower() + " (id_note bigserial NOT NULL, text_notes text, term text, date_added character varying(16), PRIMARY KEY(id_note))");
            res = name.toLower();
        }
        else
        {
            res = query.value(0).toString();

            //Если есть информация о существовании таблицы, но по каким-либо причинам сама таблица отсутствует - создаем
            query.exec("SELECT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name = '" + res + "')");
            if (query.next())
            {
                control = query.value(0).toBool();
                if (!control)
                {
                    query.exec("CREATE TABLE IF NOT EXISTS " + res + " (id_note bigserial NOT NULL, text_notes text, term text, date_added character varying(16), PRIMARY KEY(id_note))");
                }
            }
        }
    }

    return res;
}

void Notes::slotEnterButton() //Добавление заметки
{
    //Получаем дату добавления
    QString date_added = QDate::currentDate().toString(Qt::SystemLocaleShortDate) + " " + QTime::currentTime().toString(Qt::SystemLocaleShortDate);
    QString text = te_text->toPlainText(); //Текста из полей
    QString term = te_term->toPlainText();

    if (text != "" && te_term->toPlainText() != "") //Если поля не пустые
    {
        //Добавляем в таблицу
        query.exec("INSERT INTO " + tabName + " (text_notes, term, date_added) VALUES ('" + text + "', '" + term + "', '" + date_added + "')");

        te_text->clear();       //Очищаем поля
        te_term->clear();
        te_text->setFocus();    //Фокус на поле текста

        RefreshTable();         //Обновление таблицы
    }
    else
    {
        popNotify->setPopupText("Заполните поля");
        popNotify->show(parentForm);
    }

}

void Notes::slotCustomMenuRequested(QPoint pos) //Слот меню в таблице
{
    QMenu * menu = new QMenu(this);
    QAction * deleteDevice = new QAction(("Удалить запись из таблицы"), this);
    connect(deleteDevice, SIGNAL(triggered()), this, SLOT(slotRemoveRecord())); // Обработчик удаления записи
    menu->addAction(deleteDevice);
    menu->popup(tv_table->viewport()->mapToGlobal(pos));
}

void Notes::slotRemoveRecord() //Слот удаледения таблицы
{
    //Выясняем, какая из строк была выбрана
    int row = tv_table->selectionModel()->currentIndex().row();
    if (row >= 0)
    {
        //Производим удаление записи.
        if(!modelDevice->removeRow(row))
        {
            QMessageBox::warning(this, ("Уведомление"),
                                 ("Не удалось удалить запись.\n"
                                  "Возможно она используется другими таблицами.\n"));
        }

        RefreshTable();
    }
}
