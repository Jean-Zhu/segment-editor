// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ApplicationSettings.hpp"
#include <score_git_info.hpp>

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QList>
#include <QObject>
#include <QString>
namespace score
{
void ApplicationSettings::parse(QStringList cargs, int& argc, char** argv)
{
  QCommandLineParser parser;
  parser.setApplicationDescription(QObject::tr(
      "score - An interactive sequencer for the intermedia arts."));
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument(
      "file", QCoreApplication::translate("main", "Scenario to load."));

  QCommandLineOption noGUI(
      "no-gui", QCoreApplication::translate("main", "Disable GUI"));
  parser.addOption(noGUI);
  QCommandLineOption noRestore(
      "no-restore",
      QCoreApplication::translate("main", "Disable auto-restore"));
  parser.addOption(noRestore);

  QCommandLineOption autoplayOpt(
      "autoplay",
      QCoreApplication::translate("main", "Auto-play the loaded scenario"));
  parser.addOption(autoplayOpt);

  if(cargs.contains("--help") || cargs.contains("--version"))
  {
    QCoreApplication app(argc, argv);
    setQApplicationMetadata();
    parser.process(cargs);
    exit(0);
  }
  else
  {
    parser.process(cargs);
  }

  const QStringList args = parser.positionalArguments();

  tryToRestore = !parser.isSet(noRestore);
  gui = !parser.isSet(noGUI);
  if (!gui)
    tryToRestore = false;
  autoplay = parser.isSet(autoplayOpt) && args.size() == 1;

  if (!args.empty() && QFile::exists(args[0]))
  {
    loadList.push_back(args[0]);
  }
}

void setQApplicationMetadata()
{
  QCoreApplication::setOrganizationName("SEGMent");
  QCoreApplication::setOrganizationDomain("segment.com");
  QCoreApplication::setApplicationName("SEGMent");
  QCoreApplication::setApplicationVersion(QString("%1.%2")
                                          .arg(SCORE_VERSION_MAJOR)
                                          .arg(SCORE_VERSION_MINOR));
}

}
