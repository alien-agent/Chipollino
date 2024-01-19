#pragma once
#include <deque>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "Objects/FiniteAutomaton.h"
#include "Objects/Grammar.h"
#include "Objects/Regex.h"

// Типизация входныx данных
namespace Typization {

// Перечисление типов объектов
enum class ObjectType {
	NFA,		 // недетерминированный КА
	DFA,		 // детерминированный КА
	Regex,		 // регулярное выражение
	RandomRegex, // место для подстановки сгенерированных регулярных выражений
	Int,		 // целое число
	String,		 // имя файла для чтения
	Boolean,	 // true/false
	OptionalBool,	// optional<bool>
	AmbiguityValue, // yes/no/ы/ь
	PrefixGrammar,	// префиксная грамматика
	Array,			// массив
};

// Структуры объектов для хранения в интерпретаторе
template <ObjectType T, class V> struct ObjectHolder {
	V value;
	ObjectType type() const {
		return T;
	}

	ObjectHolder() {}
	explicit ObjectHolder(V value) : value(value) {}
};

// Сами структуры
struct ObjectNFA;
struct ObjectDFA;
struct ObjectRegex;
struct ObjectInt;
struct ObjectString;
struct ObjectBoolean;
struct ObjectOptionalBool;
struct ObjectAmbiguityValue;
struct ObjectPrefixGrammar;
struct ObjectArray;

// Универсальный объект
using GeneralObject =
	std::variant<ObjectNFA, ObjectDFA, ObjectRegex, ObjectInt, ObjectString, ObjectBoolean,
				 ObjectOptionalBool, ObjectAmbiguityValue, ObjectPrefixGrammar, ObjectArray>;

#define OBJECT_DEFINITION(type, value)                                                             \
	struct Object##type : public ObjectHolder<ObjectType::type, value> {                           \
		using ObjectHolder::ObjectHolder;                                                          \
	};

// Определение структур объектов
OBJECT_DEFINITION(NFA, FiniteAutomaton)
OBJECT_DEFINITION(DFA, FiniteAutomaton)
OBJECT_DEFINITION(Regex, Regex)
OBJECT_DEFINITION(Int, int)
OBJECT_DEFINITION(String, std::string)
OBJECT_DEFINITION(Boolean, bool)
OBJECT_DEFINITION(OptionalBool, std::optional<bool>)
OBJECT_DEFINITION(AmbiguityValue, FiniteAutomaton::AmbiguityValue)
OBJECT_DEFINITION(PrefixGrammar, PrefixGrammar)
OBJECT_DEFINITION(Array, std::vector<GeneralObject>)

// перевод ObjectType в string (для логирования и дебага)
inline static const std::unordered_map<ObjectType, std::string> types_to_string = {
	{ObjectType::NFA, "NFA"},
	{ObjectType::DFA, "DFA"},
	{ObjectType::Regex, "Regex"},
	{ObjectType::RandomRegex, "RandomRegex"},
	{ObjectType::Int, "Int"},
	{ObjectType::String, "String"},
	{ObjectType::Boolean, "Boolean"},
	{ObjectType::OptionalBool, "OptionalBool"},
	{ObjectType::AmbiguityValue, "AmbiguityValue"},
	{ObjectType::PrefixGrammar, "PrefixGrammar"},
	{ObjectType::Array, "Array"},
};

// вложенные типы данных:
inline static const std::unordered_map<ObjectType, std::vector<ObjectType>> types_parents = {
	{ObjectType::DFA, {ObjectType::NFA}}};
inline static const std::unordered_map<ObjectType, std::vector<ObjectType>> types_children = {
	{ObjectType::NFA, {ObjectType::DFA}}};

// используется, чтобы получить всех возможных детей / родителей типа
static std::vector<ObjectType> get_types(
	ObjectType type, const std::unordered_map<ObjectType, std::vector<ObjectType>>& other) {
	std::vector<ObjectType> res = {type};
	if (other.count(type))
		for (ObjectType t : other.at(type))
			res.push_back(t);
	return res;
}

// проверка на прринадлежность элемента вектору
static bool is_belong(const std::vector<ObjectType>& vec, ObjectType value) {
	return std::find(vec.begin(), vec.end(), value) != vec.end();
}

// преообразование типа (мб можно покреативнее)
static GeneralObject convert_type(const GeneralObject& obj, ObjectType type) {
	if (std::holds_alternative<ObjectDFA>(obj) && type == ObjectType::NFA)
		return ObjectNFA(std::get<ObjectDFA>(obj).value);
	return obj;
}

}; // namespace Typization

/*
Заика в бар заходит, подходит к стойке
— Ддай ммнее крружжкку ппива.
Бармен:
— Ппожаллусттта.
— Бблаггоддаррю.
Подходит другой человек:
— Кружку пива.
Бармен:
— Пожалуйста.
— Благодарю.
Снова подходит заика:
— Тты чче, нна ддо ммной пприккалывваешшся.
— Дда ннет, этто я ннад нним пприкколлолся
*/