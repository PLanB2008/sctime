#include "setupdsm.h"

#ifdef WIN32
# include <windows.h>
# include <lmcons.h> // UNLEN
#else
# include <unistd.h>
#endif
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlError>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include "sctimexmlsettings.h"
#include "datasource.h"
#include "globals.h"

DatasourceManager* kontenDSM;
DatasourceManager* bereitDSM;

static const
QString kontenQuery(
  "Select  "
  "   gb.name, " // 0
  "   team.kostenstelle, "
  "   konto.name,  "
  "   f_username(konto.verantwortlich), " // 3
  "   f_username(coalesce(konto.stellvertreter, konto.verantwortlich)), "
  "   konto.abgerechnet_bis, "
  "   konto.zeitlimit, "  // 6
  "   u.name, "
  "   f_username(coalesce(u.verantwortlich, konto.verantwortlich)), "
  "   f_username(coalesce(u.stellvertreter, u.verantwortlich, konto.verantwortlich)), " // 9
  "   coalesce(unterkonto_art.name || ' (' || u.art || ')', u.art), "
  "   coalesce(u.beschreibung, '') || coalesce('; noch nicht abgerechnet: ' || (get_budget_saldo(u.unterkonto_id)::numeric(8,2)), ''), "
  "   coalesce(uk.kommentar, '') " // 12
  "From "
  "  gb "
  "  join konto on (gb.gb_id = konto.gb_id) "
  "  join team on (team.team_id = konto.team_id)  "
  "  join unterkonto u on (u.konto_id = konto.konto_id) "
  "  join unterkonto_art on (u.art = unterkonto_art.art) "
  "  left join unterkonto_kommentar uk on (u.unterkonto_id = uk.unterkonto_id) "
  "Where "
  " u.eintragbar "
  "Order By gb.name, konto.name, u.name, uk.kommentar ");

static QString username() {
  static QString result;
  if (!result.isNull())
    return result;
#ifdef WIN32
    char winUserName[UNLEN + 1];
    DWORD winUserNameSize = sizeof(winUserName);
    if (GetUserNameA(winUserName, &winUserNameSize))
      result = QString::fromLocal8Bit(winUserName);
#else
    char *login = getlogin();
    if (login)
      result = QString::fromLocal8Bit(login);
#endif
    if (result.isEmpty()) {
      result = "";
      logError(QObject::tr("Der Benutzername kann nicht festgestellt werden."));
    }
    return result;
}

static QString password() {
  static QString result;
  if (!result.isNull())
    return result;
  result = username(); // the username is the default password
  // try to read password from a file
  QList<QString> pwdfilepaths;
  pwdfilepaths << QDir::homePath() + QDir::separator() + ".Zeit";
#ifdef WIN32
  // try drive H: on Windows
  pwdfilepaths << "H:\\.Zeit";
#endif
  QString p;
  foreach (p, pwdfilepaths) {
    QFile pwdfile(p);
    if (pwdfile.open(QIODevice::ReadOnly)) {
      QTextStream qs(&pwdfile);
      result = qs.readLine();
      break;
    } else
      logError(QObject::tr("Beim Lesen aus Datei %1: %2").arg(p, pwdfile.errorString()));
  }
  return result;
}

static const QString bereitQuery("SELECT kategorie, beschreibung FROM v_bereitschaft_sctime");

void setupDatasources(const QStringList& datasourceNames,
                      const SCTimeXMLSettings& settings,
                      const QString &kontenPath, const QString &bereitPath)
{
  kontenDSM = new DatasourceManager();
  bereitDSM = new DatasourceManager();
  trace(QObject::tr("verfügbare Datenbanktreiber: %1.").arg(QSqlDatabase::drivers().join(", ")));
  if (!kontenPath.isEmpty())
    kontenDSM->sources.append(new FileReader(kontenPath, "|", 13));
  if (!bereitPath.isEmpty())
    bereitDSM->sources.append(new FileReader(bereitPath, "|", 2));
  QString dsname;
  foreach (dsname, datasourceNames) {
    if (dsname.compare("file") == 0) {
      kontenDSM->sources.append(new FileReader(configDir + "/zeitkonten.txt", "|", 13));
      bereitDSM->sources.append(new FileReader("zeitbereitls.txt", "|", 2));
    } else if (dsname.compare("command") == 0) {
#ifdef WIN32
      logError(QObject::tr("Datenquelle 'command' ist unter Windows nicht verfügbar"));
#else
      kontenDSM->sources.append(new CommandReader("zeitkonten --mikrokonten --separator='|'", "|", 13));
      bereitDSM->sources.append(new CommandReader("zeitbereitls --separator='|'", "|", 2));
#endif
    } else {
      if (!QSqlDatabase::drivers().contains(dsname)) {
        logError(QObject::tr("Datenbanktreiber oder Datenquelle nicht verfügbar: ") + dsname);
        continue;
      }
      QSqlDatabase db = QSqlDatabase::addDatabase(dsname, dsname);
      if (!db.isValid() || db.isOpenError()) {
        logError(QObject::tr("data source '%1'not working: %2").arg(dsname, db.lastError().driverText()));
        continue;
      }
      db.setDatabaseName(dsname.startsWith("QODBC") ? "DSN=Postgres_Zeit" : "zeit");
      db.setHostName("zeitdabaserv");
      db.setUserName(username());
      db.setPassword(password());
      kontenDSM->sources.append(new SqlReader(db, kontenQuery));
      bereitDSM->sources.append(new SqlReader(db,  bereitQuery));
    }
  }
}
