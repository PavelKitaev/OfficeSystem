#ifndef LOG_H
#define LOG_H

#include <QString>
#include <QTextStream>
#include <QFile>
#include <QDateTime>

class Log
{
private:
  QString filename; //Имя файла

public:
  Log();
  void WriteMsg(QString msg); //Запись данных в файл
};

#endif // LOG_H
