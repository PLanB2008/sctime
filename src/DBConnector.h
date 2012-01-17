#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H

#include <QSqlDatabase>
#include <QString>

class DBConnector {
  public:
    DBConnector();
    QSqlDatabase open();
  private:
    QSqlDatabase connect(const char* driver, const char* dbname);
    QString m_username;
    QString m_password;
};

#endif
