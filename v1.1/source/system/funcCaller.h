/*
 オリジナルコード：http://dixq.net/forum/viewtopic.php?f=3&t=754
 Justy氏に感謝！
 */



/*
呼び出し方
 Caller caller;

 caller.SetFunction(&NormalFunc);
 caller.Execute();

 classB1 b1;
 caller.SetFunction(b1, &classB1::b1func);
 caller.Execute();

 classB2 b2;
 caller.SetFunction(b2, &classB2::b2func);
 caller.Execute();

*/


// ----- 汎用呼び出しクラス
class Caller
{
public:

	// コンストラクタ
	Caller()	: m_holder()	{}

	template <class T>
	Caller(T &instance, void (T::*func)(UCHAR))	:m_holder()
	{
		delete m_holder;
		m_holder = new MFuncHolder<T>(instance, func);
	}

	Caller(void (*func)(UCHAR))	:m_holder()
	{
		delete m_holder;
		m_holder = new FuncHolder(func);
	}

	// デストラクタ
	~Caller()
	{
		delete m_holder;
	}

	// メンバ関数設定
	template <class T>
	void SetFunction(T &instance, void (T::*func)(UCHAR))
	{
		delete m_holder;
		m_holder = new MFuncHolder<T>(instance, func);
	}

	// 関数設定
	void SetFunction(void (*func)(UCHAR))
	{
		delete m_holder;
		m_holder = new FuncHolder(func);
	}

	// 呼び出し
	void Execute(UCHAR uc)
	{
		if(m_holder){
			m_holder->Execute(uc);
		}
	}


private:

	class HolderBase
	{
	public:
		virtual ~HolderBase()
		{
		}

		virtual void Execute(UCHAR) = 0;
	};

	// メンバ関数用
	template<class T>
	class MFuncHolder
		: public HolderBase
	{
	public:
		MFuncHolder(T &instance, void (T::*func)(UCHAR))
		: m_instance(instance), m_pMFunc(func)
		{
		}

	public:
		virtual void Execute(UCHAR uc)
		{
			if(m_pMFunc){
				(m_instance.*m_pMFunc)(uc);
			}
		}

	private:
		T&		  m_instance;
		void (T::*m_pMFunc)(UCHAR);

		MFuncHolder& operator=(const MFuncHolder &);
	};

	// 関数用
	class FuncHolder
		: public HolderBase
	{
	public:
		FuncHolder(void (*func)(UCHAR))
		: m_pFunc(func)
		{
		}

	public:
		virtual void Execute(UCHAR uc)
		{
			if(m_pFunc){
				(*m_pFunc)(uc);
			}
		}

	private:
		void (*m_pFunc)(UCHAR);
	};

	HolderBase *		m_holder;

	// 一応コピー禁止
	Caller& operator=(const Caller &);
	Caller(const Caller &);
};
