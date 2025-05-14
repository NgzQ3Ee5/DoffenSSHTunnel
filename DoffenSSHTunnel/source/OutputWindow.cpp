/*
 *   File name: OutputWindow.cpp
 *   Summary:	Terminal-like window to watch output of an external process
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include <QApplication>
#include <QCloseEvent>
#include <QTimer>
#include <QSettings>
#include <QDebug>

#include "OutputWindow.h"

// using QDirStat::readColorEntry;
// using QDirStat::writeColorEntry;
// using QDirStat::readFontEntry;
// using QDirStat::writeFontEntry;

OutputWindow::OutputWindow(QWidget *parent)
    : QDialog(parent), _ui(new Ui::OutputWindow), _showOnStderr(true),
      _noMoreProcesses(false), _closed(false), _killedAll(false),
      _errorCount(0), _exitCode(0) {
  _ui->setupUi(this);
  qDebug() << __FUNCTION__ << "Creating" << Qt::endl;
  readSettings();

  _ui->terminal->clear();
  _ui->resetZoomButton->setVisible(false);
  _ui->closeButton->setVisible(false);
  setAutoClose(false);
  hideSearch();

  connect(_ui->actionZoomIn,     &QAction::triggered, this, &OutputWindow::zoomIn);
  connect(_ui->actionZoomOut,    &QAction::triggered, this, &OutputWindow::zoomOut);
  connect(_ui->actionResetZoom,  &QAction::triggered, this, &OutputWindow::resetZoom);
  connect(_ui->actionKillProcess,&QAction::triggered, this, &OutputWindow::killAll);
  connect(_ui->actionShowSearch, &QAction::triggered, this, &OutputWindow::showSearch);
  connect(_ui->actionHideSearch, &QAction::triggered, this, &OutputWindow::hideSearch);
  connect(_ui->actionSearch,     &QAction::triggered, this, &OutputWindow::search);
  connect(_ui->actionSearchPrev, &QAction::triggered, this, &OutputWindow::searchPrev);
  connect(_ui->actionSeachNext,  &QAction::triggered, this, &OutputWindow::searchNext);
  connect(_ui->wrapCheckbox,     &QCheckBox::toggled, this, &OutputWindow::enableWrap);

  updateActions();
}

OutputWindow::~OutputWindow() {
  qDebug() << __FUNCTION__ << "Destructor" << Qt::endl;

  if (!_processList.isEmpty()) {
    qWarning() << _processList.size() << " processes left over" << Qt::endl;

    foreach ( QProcess * process, _processList )
        qWarning() << "Left over: " << process << Qt::endl;

    qDeleteAll(_processList);
  }

  //writeSettings();
}

void OutputWindow::addProcess(QProcess *process) {
  //    CHECK_PTR( process );

  if (_killedAll) {
    qInfo() << "User killed all processes - " << "no longer accepting new processes" << Qt::endl;
    process->kill();
    process->deleteLater();
  }

  _processList << process;
  qDebug() << "Adding process" << Qt::endl;

  connect(process, &QProcess::readyReadStandardOutput, this, &OutputWindow::readStdout);
  connect(process, &QProcess::readyReadStandardError,  this, &OutputWindow::readStderr);
  connect(process, &QProcess::errorOccurred,           this, &OutputWindow::processError);
  connect(process, &QProcess::finished,                this, &OutputWindow::processFinished);

  if (!hasActiveProcess())
    startNextProcess();
}

void OutputWindow::addCommandLine(const QString commandline) {
  addText(commandline, _commandTextColor);
}

void OutputWindow::addStdout(const QString output) {
  addText(output, _stdoutColor);
}

void OutputWindow::addStderr(const QString output) {
  _errorCount++;
  addText(output, _stderrColor);
  qWarning() << output << ( output.endsWith( "\n" ) ? "" : "\n" );

  if (_showOnStderr && !isVisible() && !_closed)
    show();
}

void OutputWindow::addText(const QString &rawText, const QColor &textColor) {
  if (rawText.isEmpty())
    return;

  QString text = rawText;

  if (!text.endsWith("\n"))
    text += "\n";

  _ui->terminal->moveCursor(QTextCursor::End);
  QTextCursor cursor(_ui->terminal->textCursor());

  QTextCharFormat format;
  format.setForeground(QBrush(textColor));
  cursor.setCharFormat(format);
  cursor.insertText(text);
}

void OutputWindow::clearOutput() { _ui->terminal->clear(); }

QProcess *OutputWindow::senderProcess(const char *function) const {
  QProcess *process = qobject_cast<QProcess *>(sender());

  if (!process) {
    if (sender()) {
      qWarning() << "Expecting QProcess as sender() in " << function
                     <<" , got "
                     << sender()->metaObject()->className() << Qt::endl;
    } else {
      qWarning() << "NULL sender() in " << function << Qt::endl;
    }
  }

  return process;
}

void OutputWindow::readStdout() {
  QProcess *process = senderProcess(__FUNCTION__);

  if (process)
    addStdout(QString::fromUtf8(process->readAllStandardOutput()));
}

void OutputWindow::readStderr() {
  QProcess *process = senderProcess(__FUNCTION__);

  if (process)
    addStderr(QString::fromUtf8(process->readAllStandardError()));
}

void OutputWindow::processFinished(int exitCode,
                                   QProcess::ExitStatus exitStatus) {
  switch (exitStatus) {
  case QProcess::NormalExit:
    qDebug() << "Process finished normally." << Qt::endl;
    addCommandLine(tr("Process finished."));
    break;

  case QProcess::CrashExit:

    if (exitCode == 0) {
      // Don't report an exit code of 0: Since we are starting all
      // processes with a shell, that exit code would be the exit
      // code of the shell; that would only be useful if the shell
      // crashed or could not be started.

      qWarning() << "Process crashed." << Qt::endl;
      addStderr(tr("Process crashed."));
    } else {
      qWarning() << "Process crashed. Exit code: " << exitCode <<
      Qt::endl;
      addStderr(tr("Process crashed. Exit code: %1").arg(exitCode));
    }
    break;
  }

  if(exitCode != 0) {
      _exitCode = exitCode;
  }

  QProcess *process = senderProcess(__FUNCTION__);

  if (process) {
    _processList.removeAll(process);

    if (_processList.isEmpty() && _noMoreProcesses) {
      qDebug() << "Emitting lastProcessFinished() err: " <<
              _errorCount << Qt::endl;
      emit lastProcessFinished(_errorCount);
    }

    process->deleteLater();
    closeIfDone();
  }

  startNextProcess(); // this also calls updateActions()
}

void OutputWindow::processError(QProcess::ProcessError error) {
  QString msg;

  switch (error) {
  case QProcess::FailedToStart:
    msg = tr("Error: Process failed to start.");
    break;

  case QProcess::Crashed: // Already reported via processFinished()
    break;

  case QProcess::Timedout:
    msg = tr("Error: Process timed out.");
    break;

  case QProcess::ReadError:
    msg = tr("Error reading output from the process.");
    break;

  case QProcess::WriteError:
    msg = tr("Error writing data to the process.");
    break;

  case QProcess::UnknownError:
    msg = tr("Unknown error.");
    break;
  }

  if (!msg.isEmpty()) {
    qWarning() << msg << Qt::endl;
    addStderr(msg);
  }

  QProcess *process = senderProcess(__FUNCTION__);

  if (process) {
    _processList.removeAll(process);

    if (_processList.isEmpty() && _noMoreProcesses) {
      qDebug() << "Emitting lastProcessFinished() err: " <<
              _errorCount << Qt::endl;
      emit lastProcessFinished(_errorCount);
    }

    process->deleteLater();
  }

  startNextProcess(); // this also calls updateActions()

  if (!_showOnStderr && !isVisible())
    closeIfDone();
}

void OutputWindow::closeIfDone() {
  if (_processList.isEmpty() && _noMoreProcesses) {
    if ((autoClose() && _errorCount == 0 && _exitCode == 0) || _closed || !isVisible()) {
      qDebug() << "No more processes to watch. Auto-closing." << Qt::endl;
      this->deleteLater(); // It is safe to call this multiple times
    }
  }
}

void OutputWindow::noMoreProcesses() {
  _noMoreProcesses = true;

  if (_processList.isEmpty() && _noMoreProcesses) {
    qDebug() << "Emitting lastProcessFinished() err: " << _errorCount << Qt::endl;
    emit lastProcessFinished(_errorCount);
  }

  closeIfDone();
}

void OutputWindow::zoom(double factor) {
  QFont font = _ui->terminal->font();

  if (font.pixelSize() != -1) {
    int pixelSize = qRound(font.pixelSize() * factor);
    qDebug() << "New font size: " << pixelSize << " pixels" << Qt::endl;
    font.setPixelSize(pixelSize);
  } else {
    qreal pointSize = font.pointSize() * factor;
    qDebug() << "New font size: " << pointSize << " points" << Qt::endl;
    font.setPointSize(qRound(pointSize));
  }

  _ui->terminal->setFont(font);
}

void OutputWindow::zoomIn() { zoom(1.1); }

void OutputWindow::zoomOut() { zoom(1.0 / 1.1); }

void OutputWindow::resetZoom() {
  //qDebug() << "Resetting font to normal" << Qt::endl;
  // TODO _ui->terminal->setFont( _terminalDefaultFont );
}

void OutputWindow::enableWrap(bool enable) {
    if(enable) {
        //_ui->terminal->setLineWrapMode(QTextEdit::LineWrapMode::WidgetWidth);
        _ui->terminal->setWordWrapMode(QTextOption::WrapMode::WrapAtWordBoundaryOrAnywhere);
    } else {
        //_ui->terminal->setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);
        _ui->terminal->setWordWrapMode(QTextOption::WrapMode::NoWrap);
    }
    _ui->wrapCheckbox->setChecked(enable);
}

void OutputWindow::killAll() {
  int killCount = 0;

  foreach (QProcess *process, _processList) {
    qInfo() << "Killing process " << process << Qt::endl;
    process->kill();
    _processList.removeAll(process);
    process->deleteLater();
    ++killCount;
  }

  _killedAll = true;
  addCommandLine(killCount == 1 ? tr("Process killed.")
                                : tr("Killed %1 processes.").arg(killCount));
}

#if 0
void OutputWindow::setTerminalBackground( const QColor & newColor )
{
    // TO DO
    // TO DO
    // TO DO
}
#endif

bool OutputWindow::hasActiveProcess() const {
  foreach (QProcess *process, _processList) {
    if (process->state() == QProcess::Starting ||
        process->state() == QProcess::Running) {
      return true;
    }
  }

  return false;
}

QProcess *OutputWindow::pickQueuedProcess() {
  foreach (QProcess *process, _processList) {
    if (process->state() == QProcess::NotRunning)
      return process;
  }

  return 0;
}

QProcess *OutputWindow::startNextProcess() {
  QProcess *process = pickQueuedProcess();

  if (process) {
    QString dir = process->workingDirectory();

    if (dir != _lastWorkingDir) {
      addCommandLine("cd " + dir);
      _lastWorkingDir = dir;
    }

    addCommandLine(command(process));
    qInfo() << "Starting: " << process << Qt::endl;

    if(process->program().isEmpty()) {
        process->start(process->arguments().join(" "));
    } else {
        process->start();
    }

    qApp->processEvents(); // Keep GUI responsive
  }

  updateActions();

  return process;
}

QString OutputWindow::command(QProcess *process) {

  QString prog = process->program();
  QStringList args = process->arguments();

  QStringList ret;
  if(!prog.isNull() && !prog.isEmpty()) {
      ret.append(prog);
  }
  if(!args.isEmpty()) {
      ret.append(args);
  }
  return ret.join(" ");
}

bool OutputWindow::autoClose() const {
  return _ui->autoCloseCheckBox->isChecked();
}

void OutputWindow::setAutoCloseVisible(bool visible) {
  _ui->autoCloseCheckBox->setVisible(visible);
}

void OutputWindow::setAutoClose(bool autoClose) {
  _ui->autoCloseCheckBox->setChecked(autoClose);
}

void OutputWindow::closeEvent(QCloseEvent *event) {
    handleCloseOrReject();
    event->accept();
}

void OutputWindow::reject() {
    handleCloseOrReject();
    QDialog::reject();
}

void OutputWindow::handleCloseOrReject() {
    killAll();
    _closed = true;

    if (_processList.isEmpty() && _noMoreProcesses)
      this->deleteLater();

    // If there are any more processes, wait until the last one is finished and
    // then deleteLater().

    writeSettings();
}


void OutputWindow::updateActions() {
  _ui->killButton->setEnabled(hasActiveProcess());
}

void OutputWindow::showAfterTimeout(int timeoutMillisec) {
  if (timeoutMillisec <= 0)
    timeoutMillisec = _defaultShowTimeout;

  QTimer::singleShot(timeoutMillisec, this, SLOT(timeoutShow()));
}

void OutputWindow::timeoutShow() {
  if (!isVisible() && !_closed)
    show();
}

void OutputWindow::showSearch() {
    _ui->searchWidget->setVisible(true);
    _ui->searchEdit->setFocus(Qt::FocusReason::NoFocusReason);
    _ui->searchEdit->selectAll();
}

void OutputWindow::hideSearch() {
    qDebug() << "hideSearch";
    _ui->searchWidget->setVisible(false);
    _ui->searchEdit->setText("");
}

/**
 * @brief search while typing
 */
void OutputWindow::search() {
    qDebug() << "search";
    if(!_cursor.isNull()) {
        _ui->terminal->setTextCursor(_cursor);
    }
    QTextCursor cursor = _ui->terminal->textCursor();
    bool found = findString(_ui->searchEdit->text(), false, false, false);
    if(found) {
        _cursor = cursor;
    } else {
        _cursor = QTextCursor();
    }
}

/**
 * @brief hitting Find Previous
 */
void OutputWindow::searchPrev() {
    qDebug() << "searchPrev";
    findString(_ui->searchEdit->text(), true, false, false);
}

/**
 * @brief hitting Find Next or Enter key in the search box
 */
void OutputWindow::searchNext() {
    qDebug() << "searchNext";
    findString(_ui->searchEdit->text(), false, false, false);
}


bool OutputWindow::findString(QString s, bool reverse, bool casesens, bool words)
{
    QTextDocument::FindFlags flag;
    if (reverse) flag |= QTextDocument::FindBackward;
    if (casesens) flag |= QTextDocument::FindCaseSensitively;
    if (words) flag |= QTextDocument::FindWholeWords;

    QTextCursor cursor = _ui->terminal->textCursor();
    // here , you save the cursor position
    QTextCursor cursorSaved = cursor;

    bool found = _ui->terminal->find(s, flag);
    if (!found)
    {
        //nothing is found | jump to start/end
        cursor.movePosition(reverse?QTextCursor::End:QTextCursor::Start);

        //following line :
        // the cursor is set at the beginning/end of the document (if search is reverse or not)
        // in the next "find", if the word is found, now you will change the cursor position
        _ui->terminal->setTextCursor(cursor);

        found = _ui->terminal->find(s, flag);
        if (!found)
        {
            // word not found : we set the cursor back to its initial position
            _ui->terminal->setTextCursor(cursorSaved);
        }
    }

    return found;
}


void OutputWindow::readSettings() {
  _terminalBackground = QColor(Qt::black);
  _commandTextColor = QColor(Qt::white);
  _stdoutColor = QColor(0xff, 0xaa, 0x00);
  _stderrColor = QColor(Qt::red);
  _terminalDefaultFont = _ui->terminal->font();
  _defaultShowTimeout = 500;

  QSettings settings( g_strAppIniFile, QSettings::IniFormat);
  settings.beginGroup( "OutputWindow" );
  QPoint pos = settings.value( "pos", QPoint(0, 0) ).toPoint();
  QSize size = settings.value( "size", QSize(964, 648) ).toSize();
  int bMax = settings.value( "maximized", 0 ).toInt();
  bool bWrap = settings.value("wrap",false).toBool();
  int fontPixelSize = settings.value("fontPixelSize",0).toInt();
  int fontPointSize = settings.value("fontPointSize",0).toInt();

  if(pos.y() < 0) {
      pos = QPoint(pos.x(),1);
  } else if(pos.x() < 0) {
      pos = QPoint(1,pos.y());
  }
  if ( pos.x() && pos.y() ) move(pos);
  if ( !size.isEmpty() ) resize(size);
  if ( bMax ) showMaximized();
  enableWrap(bWrap);

  QFont font = _ui->terminal->font();
    if (font.pixelSize() != -1) {
        if(fontPixelSize > 10) {
          qDebug() << "Font size: " << fontPixelSize << " pixels" << Qt::endl;
          font.setPixelSize(fontPixelSize);
        }
    } else {
        if(fontPointSize > 4 && fontPointSize < 16) {
          qDebug() << "Font size: " << fontPointSize << " points" << Qt::endl;
          font.setPointSize(fontPointSize);
        }
    }
    _ui->terminal->setFont(font);

#if 0
  QDirStat::Settings settings;

  _terminalBackground	 = readColorEntry( settings, "TerminalBackground",
  QColor( Qt::black  ) ); _commandTextColor	 = readColorEntry( settings,
  "CommandTextColor"	 , QColor( Qt::white  ) ); _stdoutColor	 =
  readColorEntry( settings, "StdoutTextColor"	 , QColor( 0xff, 0xaa, 0x00 ) );
  _stderrColor	 = readColorEntry( settings, "StdErrTextColor"	 , QColor(
  Qt::red    ) ); _terminalDefaultFont = readFontEntry ( settings,
  "TerminalFont"	 , _ui->terminal->font() ); _defaultShowTimeout	 =
  settings.value( "DefaultShowTimeoutMillisec", 500 ).toInt();
  _ui->terminal->setFont( _terminalDefaultFont );
#endif
}

void OutputWindow::writeSettings() {

    QSettings settings( g_strAppIniFile, QSettings::IniFormat);
    settings.beginGroup( "OutputWindow" );

    int bMax = isMaximized();
    if ( !bMax )
    {
        settings.setValue( "pos", pos() );
        settings.setValue( "size", size() );
    }

    settings.setValue( "maximized", bMax );
    settings.setValue("wrap",_ui->wrapCheckbox->isChecked());

    QFont font = _ui->terminal->font();
    settings.setValue("fontPixelSize",font.pixelSize());
    settings.setValue("fontPointSize",font.pointSize());

    settings.endGroup();
    settings.sync();
#if 0
  writeColorEntry( settings, "TerminalBackground", _terminalBackground  );
  writeColorEntry( settings, "CommandTextColor"  , _commandTextColor	  );
  writeColorEntry( settings, "StdoutTextColor"   , _stdoutColor	  );
  writeColorEntry( settings, "StdErrTextColor"   , _stderrColor	  );
  writeFontEntry ( settings, "TerminalFont"	   , _terminalDefaultFont );
  settings.setValue( "DefaultShowTimeoutMillisec", _defaultShowTimeout  );
#endif
}
