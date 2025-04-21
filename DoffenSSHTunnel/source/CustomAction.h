#ifndef __CUSTOMACTION_H__
#define __CUSTOMACTION_H__

#define CUSTOM_ACTION_TYPE_COMMAND (0)
#define CUSTOM_ACTION_TYPE_FOLDER  (5)

#include <QtCore/QDataStream>
#include <QMetaType>
#include <QtCore/QString>
#include <QtCore/QUuid>

struct CustomActionStruct
{
	QUuid uUid;
	QString sLabel;
	QString sCmd;
	int iType;
	int iLevel;
	bool bExpanded;
	CustomActionStruct(const QUuid& uuid=QUuid(), const QString& label="", const QString& cmd="", int type=0, int level=0, bool expanded=false )
	{
		uUid = uuid;
		sLabel = label;
		sCmd = cmd;
		iType = type;
		iLevel = level;
		bExpanded = expanded;
	}
	bool operator==(const CustomActionStruct &other) const 
	{
		if(sLabel != other.sLabel) { return false; }
		if(sCmd != other.sCmd) { return false; }
		if(iType != other.iType) { return false; }
		if(iLevel != other.iLevel) { return false; }
		return true;
    }
	bool operator!=(const CustomActionStruct &other) const 
	{
		return !(*this == other);
    }
};

Q_DECLARE_METATYPE(CustomActionStruct)

/*
	NOTE! The streaming operators gets declared in main.cpp main() method like this:
	qRegisterMetaTypeStreamOperators<CustomActionStruct>("CustomActionStruct");
	qRegisterMetaType<CustomActionStruct>("CustomActionStruct");
*/

inline QDataStream &operator>>(QDataStream &s, CustomActionStruct &p)
{
	QUuid uUid;
	QString label;
	QString cmd;
	quint32 type;
	quint32 level;
	bool expanded;
	s >> uUid >> label >> cmd >> type >> level >> expanded;
	p = CustomActionStruct(uUid,label,cmd,type,level,expanded);
	return s;
}

inline QDataStream &operator<<(QDataStream &s, const CustomActionStruct &p)
{
	s << p.uUid << p.sLabel << p.sCmd << quint32(p.iType) << quint32(p.iLevel) << p.bExpanded;
	return s;
}

#endif
