#include "log.h"

Log::Log()
{
  filename = "logfile.log"; //Имя файла
}

void Log::WriteMsg(QString msg) //Запись данных в файл
{
  QTextStream out(stdout);
  QFile file(filename);

  if (file.open(QIODevice::Append)) //Открываем файл
    {
     QDate date = QDate::currentDate(); //Получаем текущую дату
     QTime time = QTime::currentTime(); //Получаем текущее время

     QTextStream out(&file);
     out << date.toString(Qt::SystemLocaleDate) << " " << time.toString() << " - " << msg << endl; //Записываем данные в файл
     file.close(); //Закрываем файл
    }
}
