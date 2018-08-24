#pragma once 
#include <string_view>
#include <any>
#include <boost/container_hash/hash.hpp>
#include <unordered_map>

template<typename T>
struct PropertyName : public std::string_view
{
	using value_type = T;
	using base_t = std::string_view;
	using base_t::base_t;
	operator std::string() const
	{
		return { data(),size() };
	}
};

inline namespace v2
{
	struct PropertyComponentKey
	{
		template<typename T>
		PropertyComponentKey(const PropertyName<T>& k)
			:name{ static_cast<std::string>(k) }, typeIndex{ typeid(T) }
		{
			
		}
		std::string name;
		std::type_index typeIndex;
	};
	inline bool operator==(const PropertyComponentKey& left, const PropertyComponentKey& right)
	{
		return left.typeIndex == right.typeIndex && left.name == right.name;
	}
}
namespace PropertyDetails
{
	inline void hash_combine_impl(uint32_t& h1,
		uint32_t k1)
	{
		const uint32_t c1 = 0xcc9e2d51;
		const uint32_t c2 = 0x1b873593;

		k1 *= c1;
		k1 = (k1 << 15) | (k1 >> (32 - 15));
		k1 *= c2;

		h1 ^= k1;
		h1 = (h1 << 13) | (h1 >> (32 - 13));
		h1 = h1 * 5 + 0xe6546b64;
	}

	inline void hash_combine_impl(uint64_t& h,
		uint64_t k)
	{
		const uint64_t m = UINT64_C(0xc6a4a7935bd1e995);
		const int r = 47;

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;

		// Completely arbitrary number, to prevent 0's
		// from hashing to 0.
		h += 0xe6546b64;
	}
}
namespace std
{
	template<>
	struct hash<v2::PropertyComponentKey>
	{
		constexpr size_t operator()(const v2::PropertyComponentKey& obj) const noexcept
		{
			std::size_t seed = 0x531E2887;
			PropertyDetails::hash_combine_impl(seed, std::hash<std::string>{}(obj.name));
			PropertyDetails::hash_combine_impl(seed, std::hash<std::type_index>{}(obj.typeIndex));
			return seed;
		}
	};
}

#define PROPERTY_SUBCLASS_IMPLEMENTATION(subclass)\
property_type __property_;

inline namespace v2
{
	struct PropertyInterface
	{
		virtual ~PropertyInterface() = default;
		virtual const std::type_info& target() const noexcept = 0;
		virtual void OnPropertyChanged() const noexcept {};
	};
	template<typename T, bool = std::is_scalar_v<T>>
	struct PropertyInternal;
	template<typename T>
	struct PropertyInternal<T, false> : PropertyInterface
	{
		const std::type_info& target() const noexcept override
		{
			return typeid(T);
		}
		virtual T& GetValue() noexcept = 0;
		virtual void SetValue(const T& val) = 0;
		virtual void SetValue(T&& val) = 0;
	};

	template<typename T>
	struct PropertyInternal<T, true> : PropertyInterface
	{
		const std::type_info& target() const noexcept override
		{
			return typeid(T);
		}
		virtual T GetValue() noexcept = 0;
		virtual void SetValue(T val) = 0;
	};
	class PropertyBadAccess : public std::exception
	{
		using base_t = std::exception;
	public:
		using base_t::base_t;
	};
	[[noreturn]] inline void ThrowBadSetFunctionAccesss()
	{
		throw PropertyBadAccess("can't access set function");
	}
	class PropertyComponent;
	template<typename T, bool = std::is_scalar_v<T>>
	struct FunctionProperty;
	template<typename T>
	struct FunctionProperty<T, true> : PropertyInternal<T, true>
	{
		using get_function_type = std::function<T()>;
		using set_function_type = std::function<void(T)>;
		FunctionProperty(const get_function_type& getF, const set_function_type& setF = nullptr)
			:getFunc{getF},setFunc{setF}
		{
			
		}
		FunctionProperty(get_function_type&& getF, set_function_type&& setF = nullptr)
			:getFunc{ std::move(getF) }, setFunc{ std::move(setF) }
		{

		}
		T GetValue() noexcept override
		{
			return getFunc();
		}
		void SetValue(T val) override
		{
			if (setFunc)
				setFunc(val);
			else
			{
				ThrowBadSetFunctionAccesss();
			}
		}
		get_function_type getFunc;
		set_function_type setFunc;
	};

	template<typename T>
	struct FunctionProperty<T, false> : PropertyInternal<T, false>
	{
		using get_function_type = std::function<T&()>;
		using set_copy_function_type = std::function<void(const T&)>;
		using set_move_function_type = std::function<void(T&&)>;
		FunctionProperty(const get_function_type& getF, const set_copy_function_type& setCopyF = nullptr, const set_move_function_type& setMoveF = nullptr)
			:getFunc{ getF }, setCopyFunc{ setCopyFunc }, setMoveFunc{ setMoveF }
		{

		}
		FunctionProperty(get_function_type&& getF, set_copy_function_type&& setCopyF = nullptr, set_move_function_type&& setMoveF = nullptr)
			:getFunc{ std::move(getF) }, setCopyFunc{ std::move(setCopyF) }, setMoveFunc{ std::move(setMoveF) }
		{

		}
		T& GetValue() noexcept override
		{
			return getFunc();
		}
		void SetValue(const T& val) override
		{
			if (setCopyFunc)
			{
				setCopyFunc(val);
			}
			else
			{
				ThrowBadSetFunctionAccesss();
			}
		}
		void SetValue(T&& val) override
		{
			if (setMoveFunc)
			{
				setMoveFunc(std::move(val));
			}
			else if (setCopyFunc)
			{
				setCopyFunc(val);
			}
			else
			{
				ThrowBadSetFunctionAccesss();
			}
		}
		std::function<T&()> getFunc;
		std::function<void(const T&)> setCopyFunc;
		std::function<void(T&&)> setMoveFunc;
	};

	template<typename T, bool = std::is_scalar_v<T>>
	struct ValueProperty;
	template<typename T>
	struct ValueProperty<T, true> : PropertyInternal<T, true>
	{
		ValueProperty() :value{} {}
		ValueProperty(T val) :value{ val } {}
		T GetValue() noexcept override
		{
			return value;
		}
		void SetValue(T val) override
		{
			value = val;
		}
		T value;
	};

	template<typename T>
	struct ValueProperty<T, false> : PropertyInternal<T, false>
	{
		ValueProperty(const T& val) :value{ val } {}
		ValueProperty(T&& val) :value{ std::move(val) } {}
		template<typename ... Args>
		ValueProperty(std::in_place_t, Args&&...args) : value{ std::forward<Args>(args)... } { }
		T& GetValue() noexcept override
		{
			return value;
		}
		void SetValue(const T& val) override
		{
			value = val;
		}
		void SetValue(T&& val) override
		{
			value = std::move(val);
		}
		T value;
	};

	template<typename T, bool = std::is_scalar_v<T>>
	struct ReferenceProperty;
	template<typename T>
	struct ReferenceProperty<T, true> : PropertyInternal<T, true>
	{
		ReferenceProperty(T& ref):value{std::addressof(ref)}{}
		ReferenceProperty(T* p) :value{ p } {}
		T GetValue() noexcept override
		{
			return *value;
		}
		void SetValue(T val) override
		{
			*value = val;
		}
		T* value;
	};
	template<typename T>
	struct ReferenceProperty<T, false> : PropertyInternal<T, false>
	{
		ReferenceProperty(T& ref) :value{ std::addressof(ref) } {}
		ReferenceProperty(T* p) :value{ p } {}
		T& GetValue() noexcept override
		{
			return *value;
		}
		void SetValue(const T& val) override
		{
			*value = val;
		}
		void SetValue(T&& val) override
		{
			*value = std::move(val);
		}
		T* value;
	};

	template<typename T, bool = std::is_scalar_v<T>>
	struct PropertyEventInterface;
	template<typename T>
	struct PropertyEventInterface<T, true> : PropertyInternal<T, true>
	{
		T GetValue() noexcept override
		{
			return internal_->GetValue();
		}
		void SetValue(T val) override
		{
			internal_->SetValue(val);
			SetValueEvent(val);
		}
		virtual void SetValueEvent(const T& val) const noexcept = 0;
		std::unique_ptr<PropertyInternal<T, true>> internal_;
	};
	template<typename T>
	struct PropertyEventInterface<T, false> : PropertyInternal<T, false>
	{
		T& GetValue() noexcept override
		{
			return internal_->GetValue();
		}
		void SetValue(const T& val) override
		{
			internal_->SetValue(val);
			SetValueEvent(internal_->GetValue());
		}
		void SetValue(T&& val) override
		{
			internal_->SetValue(std::move(val));
			SetValueEvent(internal_->GetValue());
		}
		virtual void SetValueEvent(const T& val) const noexcept = 0;
		std::unique_ptr<PropertyInternal<T, false>> internal_;
	};

	template<typename T>
	struct FunctionPropertyEvent : PropertyEventInterface<T>
	{
		using callback_t = std::function<void(const T&)>;
		void SetValueEvent(const T& val) const noexcept override
		{
			if (callback)
				callback(val);
		}
		void OnPropertyChanged() const noexcept override
		{
			if(callback)
				callback(const_cast<FunctionPropertyEvent*>(this)->GetValue());
			this->internal_->OnPropertyChanged();
		}
		callback_t callback;
	};
	template<typename Compare,typename CompareFrom,typename =void>
	struct PropertyEqual : std::false_type{};
	template<typename Compare, typename CompareFrom>
	struct PropertyEqual<Compare,CompareFrom,std::void_t<decltype(std::declval<Compare>() == std::declval<CompareFrom>())>> : std::true_type {};

	template<typename Compare, typename CompareFrom, typename = void>
	struct PropertyAdd : std::false_type {};
	template<typename Compare, typename CompareFrom>
	struct PropertyAdd<Compare, CompareFrom, std::void_t<decltype(std::declval<Compare>() + std::declval<CompareFrom>())>> : std::true_type {};

	template<typename Compare, typename CompareFrom, typename = void>
	struct PropertyLess : std::false_type {};
	template<typename Compare, typename CompareFrom>
	struct PropertyLess<Compare, CompareFrom, std::void_t<decltype(std::declval<Compare>() < std::declval<CompareFrom>())>> : std::true_type {};

	template<typename T, bool = std::is_scalar_v<T>>
	struct Property;
	template<typename T>
	struct Property<T, false>
	{
		Property(const Property&) = default;
		Property& operator=(const Property&) = default;
		Property& operator=(T&& val)
		{
			internal_->SetValue(std::move(val));
			return *this;
		}
		Property& operator=(const T& val)
		{
			internal_->SetValue(val);
			return *this;
		}

		void OnPropertyChanged()
		{
			internal_->OnPropertyChanged();
		}
		void setValue(T&& val)
		{
			internal_->SetValue(std::move(val));
		}
		void setValue(const T& val)
		{
			internal_->SetValue(val);
		}
		T& value()
		{
			return internal_->GetValue();
		}
		const T& value() const
		{
			return internal_->GetValue();
		}
		
		T* operator->()
		{
			return std::addressof(internal_->GetValue());
		}
		const T* operator->()const
		{
			return std::addressof(internal_->GetValue());
		}

		template<typename T1,typename = std::enable_if_t<PropertyEqual<T,T1>::value>>
		friend bool operator==(const Property& lhs, const T1& rhs)
		{
			return lhs.value() == rhs;
		}

		template<typename T1, typename = std::enable_if_t<PropertyEqual<T1,T>::value>>
		friend bool operator==(const T1& lhs, const Property& rhs)
		{
			return lhs == rhs.value();
		}

		friend bool operator!=(const Property& lhs, const T& rhs)
		{
			return !(lhs == rhs);
		}

		friend bool operator!=(const T& lhs, const Property& rhs)
		{
			return !(lhs == rhs);
		}

		template<typename T1, typename = std::enable_if_t<PropertyLess<T, T1>::value>>
		friend bool operator<(const Property& lhs, const T1& rhs)
		{
			return lhs.value() < rhs;
		}

		template<typename T1, typename = std::enable_if_t<PropertyLess<T1, T>::value>>
		friend bool operator<(const T1& lhs, const Property& rhs)
		{
			return lhs < rhs.value();
		}

		friend bool operator<=(const Property& lhs, const T& rhs)
		{
			return !(rhs < lhs);
		}

		friend bool operator<=(const T& lhs, const Property& rhs)
		{
			return !(rhs < lhs);
		}

		friend bool operator>(const Property& lhs, const T& rhs)
		{
			return rhs < lhs;
		}

		friend bool operator>(const T& lhs, const Property& rhs)
		{
			return rhs < lhs;
		}

		friend bool operator>=(const Property& lhs, const T& rhs)
		{
			return !(lhs < rhs);
		}

		friend bool operator>=(const T& lhs, const Property& rhs)
		{
			return !(lhs < rhs);
		}

		template<typename T1,typename =std::enable_if_t<PropertyAdd<T1, T>::value>>
		friend T operator+(const Property& property,const T1& val)
		{
			return property.value() + val;
		}
		template<typename T1, typename = std::enable_if_t<PropertyAdd<T，T1>::value>>
		friend T operator+(const T1& val, const Property& property)
		{
			return val + property.value();
		}
	private:
		friend PropertyComponent;
		Property(PropertyInternal<T>* p)
			:internal_{ p }
		{

		}
		PropertyInternal<T>* internal_;
	};
	template<typename T>
	struct Property<T, true>
	{
		Property(const Property&) = default;
		Property& operator=(const Property&) = default;
		Property& operator=(T value)
		{
			internal_->SetValue(value);
			return *this;
		}
		Property& operator-=(T value)
		{
			T val = static_cast<T>(*this);
			val -= value;
			return operator=(val);
		}
		Property& operator+=(T value)
		{
			T val = static_cast<T>(*this);
			val += value;
			return operator=(val);
		}
		Property& operator*=(T value)
		{
			T val = static_cast<T>(*this);
			val *= value;
			return operator=(val);
		}
		Property& operator/=(T value)
		{
			T val = static_cast<T>(*this);
			val /= value;
			return operator=(val);
		}
		Property& operator%=(T value)
		{
			T val = static_cast<T>(*this);
			val %= value;
			return operator=(val);
		}
		Property& operator&=(T value)
		{
			T val = static_cast<T>(*this);
			val &= value;
			return operator=(val);
		}
		Property& operator|=(T value)
		{
			T val = static_cast<T>(*this);
			val |= value;
			return operator=(val);
		}
		Property& operator^=(T value)
		{
			T val = static_cast<T>(*this);
			val ^= value;
			return operator=(val);
		}
		Property& operator<<=(T value)
		{
			T val = static_cast<T>(*this);
			val <<= value;
			return operator=(val);
		}
		Property& operator>>=(T value)
		{
			T val = static_cast<T>(*this);
			val >>= value;
			return operator=(val);
		}
		Property& operator++()
		{
			T val = static_cast<T>(*this);
			++val;
			return operator=(val);
		}
		Property& operator--()
		{
			T val = static_cast<T>(*this);
			--val;
			return operator=(val);
		}
		Property& operator++(int)
		{
			T val = static_cast<T>(*this);
			val++;
			return operator=(val);
		}
		Property& operator--(int)
		{
			T val = static_cast<T>(*this);
			val--;
			return operator=(val);
		}
		operator T() const
		{
			return internal_->GetValue();
		}
		T value() const
		{
			return internal_->GetValue();
		}
		T operator->()
		{
			static_assert(std::is_pointer_v<T>);
			return value();
		}
		const T operator->() const
		{
			static_assert(std::is_pointer_v<T>);
			return value();
		}
		void onPropertyChanged()
		{
		}
		friend bool operator==(const Property& lhs, T rhs)
		{
			return lhs.value() == rhs;
		}

		friend bool operator==(T lhs, const Property& rhs)
		{
			return lhs == rhs.value();
		}

		friend bool operator!=(const Property& lhs, T rhs)
		{
			return !(lhs == rhs);
		}

		friend bool operator!=(T lhs, const Property& rhs)
		{
			return !(lhs == rhs);
		}

		friend bool operator<(const Property& lhs, T rhs)
		{
			return lhs.value() < rhs;
		}

		friend bool operator<(T lhs, const Property& rhs)
		{
			return lhs < rhs.value();
		}

		friend bool operator<=(const Property& lhs, T rhs)
		{
			return !(rhs < lhs);
		}

		friend bool operator<=(T lhs, const Property& rhs)
		{
			return !(rhs < lhs);
		}

		friend bool operator>(const Property& lhs, T rhs)
		{
			return rhs < lhs;
		}

		friend bool operator>(T lhs, const Property& rhs)
		{
			return rhs < lhs;
		}

		friend bool operator>=(const Property& lhs, T rhs)
		{
			return !(lhs < rhs);
		}

		friend bool operator>=(T lhs, const Property& rhs)
		{
			return !(lhs < rhs);
		}
	private:
		friend PropertyComponent;
		Property(PropertyInternal<T>* p)
			:internal_{p}
		{
			
		}
		PropertyInternal<T>* internal_;
	};
}

inline namespace v2
{
	using property_type = std::unordered_map<PropertyComponentKey, std::unique_ptr<PropertyInterface>>;

	template<typename T>
	inline void BindFunctionProertyEventInternal(property_type::iterator iter, typename FunctionPropertyEvent<T>::callback_t&& function)
	{
		auto ptr = std::make_unique<FunctionPropertyEvent<T>>();
		ptr->callback = std::move(function);
		ptr->internal_.reset(static_cast<PropertyInternal<T>*>(iter->second.release()));
		iter->second.reset(ptr.release());
	}
	template<typename T, typename GetFunc, bool = false>
	struct MakeGetFunctionPropertyAssert;
	template<typename T, typename GetFunc>
	struct MakeGetFunctionPropertyAssert<T, GetFunc, true>
	{
		static_assert(std::is_convertible_v<typename FunctionProperty<T>::get_function_type, GetFunc>);
	};
	template<typename T, typename GetFunc>
	struct MakeGetFunctionPropertyAssert<T, GetFunc, false>
	{
		static_assert(std::is_convertible_v<typename FunctionProperty<T>::get_function_type, GetFunc>);
	};

	template<typename T, typename SetFunc, bool = false>
	struct MakeSetFunctionPropertyAssert;
	template<typename T, typename SetFunc>
	struct MakeSetFunctionPropertyAssert<T, SetFunc, true>
	{
		static_assert(std::is_convertible_v<typename FunctionProperty<T>::set_function_type, SetFunc>);
	};
	template<typename T, typename SetFunc>
	struct MakeSetFunctionPropertyAssert<T, SetFunc, false>
	{
		static_assert(std::is_convertible_v<typename FunctionProperty<T>::set_copy_function_type, SetFunc>||std::is_convertible_v<typename FunctionProperty<T>::set_move_function_type, SetFunc>);
	};


	class PropertyComponent
	{
		property_type data_;
	private:
		property_type& GetData()
		{
			return data_;
		}
		PropertyInterface* TryGetPropertyInternal(const PropertyComponentKey& key)
		{
			auto iter = GetData().find(key);
			if (iter != GetData().end())
			{
				return iter->second.get();
			}
			return nullptr;
		}
		PropertyInterface* TryGetPropertyInternal(const type_info& key)
		{
			for (auto& data : GetData())
			{
				if (data.first.typeIndex == key)
				{
					return data.second.get();
				}
			}
			return nullptr;
		}
		bool TryAddProperty(const PropertyComponentKey& key, std::unique_ptr<PropertyInterface> interface)
		{
			return GetData().emplace(key, std::move(interface)).second;
		}
		
	public:
		template<typename T>
		auto operator[](const PropertyName<T>& key)
		{
			return GetProerty(key);
		}
		template<typename T>
		auto GetProerty(const PropertyName<T>& key)
		{
			auto property = TryGetPropertyInternal(key);
			if (property != nullptr)
			{
				return Property<T>{static_cast<PropertyInternal<T>*>(property)};
			}
			throw PropertyBadAccess("can't find Proerty");
		}
		template<typename T>
		auto TryGetProperty(const PropertyName<T>& key)
		{
			PropertyInterface* property = TryGetPropertyInternal(key);
			return Property<T>{ static_cast<PropertyInternal<T>*>(property)};
		}
		template<typename T>
		auto GetProerty()
		{
			auto property = TryGetPropertyInternal(typeid(T));
			if (property != nullptr)
			{
				return Property<T>{static_cast<PropertyInternal<T>*>(property)};
			}
			throw PropertyBadAccess("can't find Proerty");
		}
		template<typename T>
		bool TryGetProperty()
		{
			auto property = TryGetPropertyInternal(typeid(T));
			return Property<T>{ static_cast<PropertyInternal<T>*>(property)};
		}

		template<typename T>
		bool BindFunctionProertyEvent(const PropertyName<T>& key, std::function<void()> function)
		{
			auto iter = GetData().find(key);
			if (iter != GetData().end())
			{
				typename FunctionPropertyEvent<T>::callback_t fn = [func = std::move(function)](const T&) {func(); };
				BindFunctionProertyEventInternal<T>(iter, std::move(fn));
				return true;
			}
			return false;
		}
		template<typename T>
		bool BindFunctionProertyEvent(const PropertyName<T>& key, typename FunctionPropertyEvent<T>::callback_t function)
		{
			auto iter = GetData().find(key);
			if (iter != GetData().end())
			{
				BindFunctionProertyEventInternal<T>(iter, std::move(function));
				return true;
			}
			return false;
		}

		template<typename T>
		bool AddValueProperty(const PropertyName<T>& key, const T& val)
		{
			return TryAddProperty(key, std::make_unique<ValueProperty<T>>(val));
		}
		template<typename T,typename ...Args>
		bool EmplaceValueProperty(const PropertyName<T>& key, Args&&... args)
		{
			if constexpr(std::is_scalar_v<T>)
			{
				return TryAddProperty(key, std::make_unique<ValueProperty<T>>(std::forward<Args>(args)...));
			}
			else
			{
				return TryAddProperty(key, std::make_unique<ValueProperty<T>>(std::in_place, std::forward<Args>(args)...));
			}
		}
		template<typename T>
		bool AddValueProperty(const PropertyName<T>& key, T&& val)
		{
			return TryAddProperty(key, std::make_unique<ValueProperty<T>>(std::move(val)));
		}

		template<typename T>
		bool AddReferenceProperty(const PropertyName<T>& key, T& val)
		{
			return TryAddProperty(key, std::make_unique<ReferenceProperty<T>>(val));
		}
		template<typename T, typename GetFunc>
		bool AddFunctionProperty(const PropertyName<T>& key, GetFunc&& getFunc)
		{
			using assert = MakeGetFunctionPropertyAssert<T, GetFunc>;
			return TryAddProperty(key, std::make_unique<FunctionProperty<T>>(std::forward<GetFunc>(getFunc)));
		}

		template<typename T, typename GetFunc,typename SetFunc>
		bool AddFunctionProperty(const PropertyName<T>& key, GetFunc&& getFunc,SetFunc&& setFunc)
		{
			using assert = MakeGetFunctionPropertyAssert<T, GetFunc>;
			using assert1 = MakeGetFunctionPropertyAssert<T, SetFunc>;
			return TryAddProperty(key, std::make_unique<FunctionProperty<T>>(std::forward<GetFunc>(getFunc), std::forward<SetFunc>(setFunc)));
		}

		template<typename T, typename GetFunc, typename SetCopyFunc,typename SetMoveFunc>
		bool AddFunctionProperty(const PropertyName<T>& key, GetFunc&& getFunc, SetCopyFunc&& setCopyFunc, SetMoveFunc&& setMoveFunc)
		{
			using assert = MakeGetFunctionPropertyAssert<T, GetFunc>;
			using assert1 = MakeGetFunctionPropertyAssert<T, SetCopyFunc>;
			using assert2 = MakeGetFunctionPropertyAssert<T, SetMoveFunc>;
			return TryAddProperty(key,
				std::make_unique<FunctionProperty<T>>(std::forward<GetFunc>(getFunc),
					std::forward<SetCopyFunc>(setCopyFunc), std::forward<SetMoveFunc>(setMoveFunc)));
		}
	};
}
