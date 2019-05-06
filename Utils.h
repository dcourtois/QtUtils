#ifndef QT_UTILS_UTILS_H
#define QT_UTILS_UTILS_H

#include "./Setup.h"

#include <QPoint>
#include <QPointF>
#include <QSize>
#include <QSizeF>
#include <QtGlobal>
#include <QtMath>


QT_UTILS_NAMESPACE_BEGIN

	//! Component wise multiplication of 2 QPointF
	constexpr QPointF operator * (const QPointF & left, const QPointF & right) { return { left.x() * right.x(), left.y() * right.y() }; }

	//! Component wise division of 2 QPointF
	constexpr QPointF operator / (const QPointF & left, const QPointF & right) { return { left.x() / right.x(), left.y() / right.y() }; }

	//! Add a scalar to each component of a QPointF
	constexpr QPointF operator + (const QPointF & left, qreal right) { return { left.x() + right, left.y() + right }; }

	//! Substract a scalar from each component of a QPointF
	constexpr QPointF operator - (const QPointF & left, qreal right) { return { left.x() - right, left.y() - right }; }

	//! Component wise multiplication of 2 QPoint
	constexpr QPoint operator * (const QPoint & left, const QPoint & right) { return { left.x() * right.x(), left.y() * right.y() }; }

	//! Component wise division of 2 QPoint
	constexpr QPoint operator / (const QPoint & left, const QPoint & right) { return { left.x() / right.x(), left.y() / right.y() }; }

	//! qRound component-wise implementation for qFloor (returns a QPoint)
	inline QPoint qFloor(const QPointF & point) { return { qFloor(point.x()), qFloor(point.y()) }; }

	//! qRound component-wise implementation for qCeil (returns a QPoint)
	inline QPoint qCeil(const QPointF & point) { return { qCeil(point.x()), qCeil(point.y()) }; }

	//! qRound component-wise implementation for QPointF (returns a QPoint)
	constexpr QPoint qRound(const QPointF & point) { return { qRound(point.x()), qRound(point.y()) }; }

	//! qMin component-wise implementation for QPoint
	constexpr QPoint qMin(const QPoint & a, const QPoint & b) { return { qMin(a.x(), b.x()), qMin(a.y(), b.y()) }; }

	//! qMin component-wise implementation for QPointF
	constexpr QPointF qMin(const QPointF & a, const QPointF & b) { return { qMin(a.x(), b.x()), qMin(a.y(), b.y()) }; }

	//! qMax component-wise implementation for QPoint
	constexpr QPoint qMax(const QPoint & a, const QPoint & b) { return { qMax(a.x(), b.x()), qMax(a.y(), b.y()) }; }

	//! qMax component-wise implementation for QPointF
	constexpr QPointF qMax(const QPointF & a, const QPointF & b) { return { qMax(a.x(), b.x()), qMax(a.y(), b.y()) }; }

	//! qBound component-wise implementation for QPoint
	constexpr QPoint qBound(const QPoint & min, const QPoint & value, const QPoint & max) { return { qBound(min.x(), value.x(), max.x()), qBound(min.y(), value.y(), max.y()) }; }

	//! qBound component-wise implementation for QPointF
	constexpr QPointF qBound(const QPointF & min, const QPointF & value, const QPointF & max) { return { qBound(min.x(), value.x(), max.x()), qBound(min.y(), value.y(), max.y()) }; }

	//! conversion from QPoint to QSize
	constexpr QSize toSize(const QPoint & point) { return { point.x(), point.y() }; }

	//! conversion from QPointF to QSizeF
	constexpr QSizeF toSize(const QPointF & point) { return { point.x(), point.y() }; }

	//! conversion from QSize to QPoint
	constexpr QPoint toPoint(const QSize & size) { return { size.width(), size.height() }; }

	//! conversion from QSizeF to QPointF
	constexpr QPointF toPoint(const QSizeF & size) { return { size.width(), size.height() }; }

QT_UTILS_NAMESPACE_END


//!
//! @def QT_UTILS_VECTOR_WRAPPER_IMPL(className, typeName, propertyName, memberName)
//!
//! This macro is used to generate the code needed to use a QVector of
//! values through a QQmlListProperty property in QML. Here is an example use:
//!
//! ```
//! class SomeClass
//! {
//!
//! 	Q_OBJECT
//! 	Q_PROPERTY(QQmlListProperty< int > intList READ intList NOTIFY intListChanged)
//!
//! signals:
//!
//! 	// this is the signal used to notify list changes
//! 	void intListChanged(QQmlListProperty< int > intList);
//!
//! public:
//!
//! 	// generate the implementation
//! 	QT_UTILS_VECTOR_WRAPPER_IMPL(SomeClass, int, intList, m_IntList)
//!
//! private:
//!
//! 	//! the internal list
//! 	QVector< int > m_IntList;
//!
//! };
//! ```
//!
#define QT_UTILS_VECTOR_WRAPPER_IMPL(className, typeName, propertyName, memberName)						\
	inline QQmlListProperty< typeName > propertyName(void)												\
	{																									\
		return QQmlListProperty< typeName >(															\
			this,																						\
			nullptr,																					\
			&className::propertyName##Append,															\
			&className::propertyName##Count,															\
			&className::propertyName##Get,																\
			&className::propertyName##Clear																\
		);																								\
	}																									\
	inline static void propertyName##Append(QQmlListProperty< typeName > * list, typeName * item)		\
	{																									\
		className * instance = static_cast< className * >(list->object);								\
		Q_ASSERT(instance != nullptr);																	\
		instance->memberName.push_back(item);															\
		instance->propertyName##Changed();																\
	}																									\
	inline static int propertyName##Count(QQmlListProperty< typeName > * list)							\
	{																									\
		className * instance = static_cast< className * >(list->object);								\
		Q_ASSERT(instance != nullptr);																	\
		return instance->memberName.size();																\
	}																									\
	inline static typeName * propertyName##Get(QQmlListProperty< typeName > * list, int index)			\
	{																									\
		className * instance = static_cast< className * >(list->object);								\
		Q_ASSERT(instance != nullptr);																	\
		Q_ASSERT(index >= 0 && index < instance->memberName.size());									\
		return instance->memberName.at(index);															\
	}																									\
	inline static void propertyName##Clear(QQmlListProperty< typeName > * list)							\
	{																									\
		className * instance = static_cast< className * >(list->object);								\
		Q_ASSERT(instance != nullptr);																	\
		instance->memberName.clear();																	\
		instance->propertyName##Changed();																\
	}																									\


#endif
