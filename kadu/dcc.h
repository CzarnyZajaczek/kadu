#ifndef DCC_H
#define DCC_H

#include <qdialog.h>
#include <qlayout.h> 
#include <qprogressbar.h>
#include <qlabel.h>
#include <qsocketnotifier.h>
#include <qhostaddress.h>

#include "kadu-config.h"
#include "libgadu.h"

extern QSocketNotifier* dccsnr;
extern QSocketNotifier* dccsnw;
extern QHostAddress config_dccip;
extern QHostAddress config_extip;
extern struct gg_dcc* dccsock;

class FileDccSocket;

class DccFileDialog : public QDialog
{
	Q_OBJECT

	public:
		enum TransferType {
			TRANSFER_TYPE_GET,
			TRANSFER_TYPE_SEND
		};

	protected:
		FileDccSocket* dccsocket;
		QLabel *l_offset;
		QProgressBar *p_progress;
		QVBoxLayout* vlayout1;
		long long int prevPercent;
		QTime *time;
		int prevOffset;
		int type;
		void closeEvent(QCloseEvent *e);

	public:
		DccFileDialog(FileDccSocket* dccsocket, TransferType type, QDialog* parent=NULL, const char* name=NULL);
		~DccFileDialog();
		void printFileInfo(struct gg_dcc* dccsock);
		void updateFileInfo(struct gg_dcc* dccsock);

		bool dccFinished;		
};

enum dccSocketState {
	DCC_SOCKET_TRANSFERRING,
	DCC_SOCKET_CONNECTION_BROKEN,
	DCC_SOCKET_TRANSFER_ERROR,
	DCC_SOCKET_TRANSFER_FINISHED,
	DCC_SOCKET_COULDNT_OPEN_FILE,
	DCC_SOCKET_TRANSFER_DISCARDED,
	DCC_SOCKET_VOICECHAT_DISCARDED
};

class DccSocket : public QObject
{
	Q_OBJECT

	protected:
		QSocketNotifier* snr;
		QSocketNotifier* snw;
		struct gg_dcc* dccsock;
		struct gg_event* dccevent;
		bool in_watchDcc;
		int State;
		static int Count;

		virtual void connectionBroken();
		virtual void dccError();
		virtual void dccEvent();
		virtual void needFileAccept();
		virtual void needFileInfo();
		virtual void noneEvent();
		virtual void dccDone();
		virtual void setState(int pstate);

	protected slots:
		void dccDataReceived();
		void dccDataSent();
	
	public:
		DccSocket(struct gg_dcc* dcc_sock);
		~DccSocket();
		virtual void initializeNotifiers();
		virtual void watchDcc(int check);
		int state();
		static int count();

	signals:
		void dccFinished(DccSocket* dcc);
};

class FileDccSocket : public DccSocket
{
	Q_OBJECT

	private:
		DccFileDialog* filedialog;
		QString selectFile();
	
	protected:
		virtual void connectionBroken();
		virtual void dccError();
		virtual void needFileAccept();
		virtual void needFileInfo();
		virtual void noneEvent();
		virtual void dccDone();
		virtual void setState(int pstate);

	public:
		FileDccSocket(struct gg_dcc* dcc_sock);
		~FileDccSocket();
};

class DccManager : public QObject
{
	Q_OBJECT

	private:
		void watchDcc();

	private slots:
		void dccSetupFailed();

	public:
		static void initModule();
		DccManager();

	public slots:
		void dccFinished(DccSocket* dcc);
		void dccReceived();
		void dccSent();
};

extern DccManager* dcc_manager;

#endif
