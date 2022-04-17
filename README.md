# Read file 1C Native AddIn

Внешний компонент для 1С:Предприятие 8 (ОС Windows x64).  

## Создано на базе:  
* Modern Native AddIn [here](https://github.com/aleksp99/ReadFileOneC).  
* ZLIB DATA COMPRESSION LIBRARY [here](https://github.com/madler/zlib).  
* RE2 [here](https://github.com/google/re2).  

## Методы:
* ВерсияКомпоненты(Version):  
	* Версия компонента в формате '1.0.0'

    AddMethod(L"", L"Значение", this, &ReadFileOneC::getValue);

* Открыть(Open):  
	* Параметры:  
		* <имя файла> (обязательный)  
		Тип: Строка.  
		Полное имя файла.  
		* <Режим> (обязательный)  
		Тип: Число.  
		1 - режим распаковки deflate.  
		2 - режим чтения lst  
		3 - режим декодирования  
	* Возвращаемое значение:  
	Тип: Булево.
	Истина - файл прочитан.
* Текст(Text):  
	* Параметры:  
		* <преобразовать> (необязательный)  
		Тип: Булево.  
		Истина - Системное представление значения в информационной базе.  
		По умолчанию: Ложь.
	* Возвращаемое значение:  
	Тип: Строка.
	Содержимое файла.
* Значение(Value):  
	* Параметры:  
		* <индексы> (обязательный)  
		Тип: Строка.  
		Адрес(индексы разделенные запятой) значения в файле.  
		* <преобразовать> (необязательный)  
		Тип: Булево.  
		Истина - Системное представление значения в информационной базе.  
		По умолчанию: Ложь.
	* Возвращаемое значение:  
		Тип: Строка.
		Значение из файла.  
* Найти(Find)  
	* Параметры:  
		* <шаблон> (необязательный)  
		Тип: Строка.  
		Шаблон регулярного выражения для сопоставления.   
		RE2 by Google  
		https://github.com/google/re2/wiki/Syntax  
	* Возвращаемое значение:  
	Тип: Строка, Число.  
	Строка - Системное представление значения в информационной базе.  
	Для преобразования в массив использовать 'ЗначениеИзСтрокиВнутр'.   
  	
## Пример:
```
СоединениеADO = Новый COMОбъект("ADODB.Connection");
СоединениеADO.Open("Provider=SQLOLEDB; Data Source=""iA11""; Initial Catalog=""DemoERP"";Trusted_Connection=Yes");
Запрос = Новый COMОбъект("ADODB.RecordSet");
Запрос.ActiveConnection = СоединениеADO;

// режим распаковки deflate
Запрос.Open("SELECT BinaryData FROM DemoERP.dbo.Params WHERE FileName = 'DBNames'");
Stream = Новый COMОбъект("ADODB.Stream");
Stream.Type = 1;
Stream.Open();
Stream.Write(Запрос.Fields(0).Value);
Stream.SaveToFile("D:\tmp1", 2);
Stream.Close();

// режим декодирования
Запрос.Open("SELECT Data FROM DemoERP.dbo.v8users WHERE Name = 'Administrator'");
Stream = Новый COMОбъект("ADODB.Stream");
Stream.Type = 1;
Stream.Open();
Stream.Write(Запрос.Fields(0).Value);
Stream.SaveToFile("D:\tmp3", 2);
Stream.Close();

// УстановитьВнешнююКомпоненту("ОбщийМакет.ReadFile1С"); для клиента
Если ПодключитьВнешнююКомпоненту("ОбщийМакет.ReadFile1С", "def") Тогда
	
	ReadFile = Новый("AddIn.def.ReadFileOneC");
	
	// режим распаковки deflate
	Если ReadFile.Открыть("D:\tmp1", 1) Тогда 
	// Если ReadFile.Открыть("C:\Program Files\1cv8\srvinfo\reg_1541\1CV8Clst.lst", 2) Тогда // режим чтения lst
	// Если ReadFile.Открыть("D:\tmp3", 3) Тогда // режим декодирования

		// {4,
		// 	{117,cf4abeab-37b2-11d4-940f-008048da11f9, .... }
		// }
		Текст = ReadFile.Текст();
		Значение1 = ReadFile.Значение("0"); // 4
		Значение2 = ReadFile.Значение("0,1"); // Неопределено
		Значение3 = ReadFile.Значение("1,0"); // 177
		Значение4 = ReadFile.Значение("1,1"); // cf4abeab-37b2-11d4-940f-008048da11f9
		Значение5 = ЗначениеИзСтрокиВнутр(ReadFile.Найти("cf4abeab-37b2-11d4-940f-[a-f0-9]{12}")); // массив - Значение5[0][0] = cf4abeab-37b2-11d4-940f-008048da11f9	
		
	КонецЕсли;	
	
КонецЕсли;
```
