修饰符
必须使用以下修饰符之一修饰每个字段：

>* required: 必须提供该字段的值，否则该消息将被视为“未初始化”。如果是在调试模式下编译 libprotobuf，则序列化一个未初始化的 message 将将导致断言失败。在优化的构建中，将跳过检查并始终写入消息。但是，解析未初始化的消息将始终失败（通过从解析方法返回 false）。除此之外，required 字段的行为与 optional 字段完全相同。
>* optional: 可以设置也可以不设置该字段。如果未设置optional字段的值，则使用默认值。对于简单类型，你可以指定自己的默认值，就像我们在示例中为电话号码类型所做的那样。否则，使用系统默认值：数字类型为 0，字符串为空字符串，bool 为 false。对于嵌入 message，默认值始终是消息的 “默认实例” 或 “原型”，其中没有设置任何字段。调用访问器以获取尚未显式设置的 optional（或 required）字段的值始终返回该字段的默认值。
>* repeated: 该字段可以重复任意次数（包括零次）。重复值的顺序将保留在 protocol buffer 中。可以将 repeated 字段视为动态大小的数组。



常见接口

```cpp
  // name
  inline bool has_name() const;
  inline void clear_name();
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline ::std::string* mutable_name();

  // id
  inline bool has_id() const;
  inline void clear_id();
  inline int32_t id() const;
  inline void set_id(int32_t value);

  // email
  inline bool has_email() const;
  inline void clear_email();
  inline const ::std::string& email() const;
  inline void set_email(const ::std::string& value);
  inline void set_email(const char* value);
  inline ::std::string* mutable_email();

  // phones
  inline int phones_size() const;
  inline void clear_phones();
  inline const ::google::protobuf::RepeatedPtrField< ::tutorial::Person_PhoneNumber >& phones() const;
  inline ::google::protobuf::RepeatedPtrField< ::tutorial::Person_PhoneNumber >* mutable_phones();
  inline const ::tutorial::Person_PhoneNumber& phones(int index) const;
  inline ::tutorial::Person_PhoneNumber* mutable_phones(int index);
  inline ::tutorial::Person_PhoneNumber* add_phones();

```



每个 message 类还包含许多其他方法，可用于检查或操作整个 message，包括：

* ```bool IsInitialized() const;```
检查是否所有 required 字段已被初始化
* ```string DebugString() const;```
 返回 message 的人类可读的表达形式，对调试特别有用
* ```void CopyFrom(const Person& from);```
 用给定的 message 的值覆盖 message
* ```void Clear();```
 将所有元素清除回 empty 状态



# 序列化与解序列化

最后，每个 protocol buffer 类都有提供通过使用 protocol buffer 的二进制格式来读写message 的方法。包括：

* ```bool SerializeToString(string* output) const;```
序列化消息并将二进制字节存储在给定的字符串中。请注意，字节是二进制的，而不是文本;我们只使用 string 类作为容器。
* ```bool ParseFromString(const string& data);```
将给定字符串解析为message
* ```bool SerializeToOstream(ostream* output) const;```
 将 message 写入给定的 C++ 的 ostream
* ```bool ParseFromIstream(istream* input);```
将给定 C++ istream 解析为message
