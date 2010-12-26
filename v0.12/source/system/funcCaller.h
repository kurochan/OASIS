/*
 �I���W�i���R�[�h�Fhttp://dixq.net/forum/viewtopic.php?f=3&t=754
 Justy���Ɋ��ӁI
 */



/*
�Ăяo����
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

#include "unsigned.h"

// ----- �ėp�Ăяo���N���X
// ----- �ėp�Ăяo���N���X
class Caller
{
public:
	
	// �R���X�g���N�^
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
	
	// �f�X�g���N�^
	~Caller()
	{
		delete m_holder;
	}
	
	// �����o�֐��ݒ�
	template <class T>
	void SetFunction(T &instance, void (T::*func)(UCHAR))
	{
		delete m_holder;
		m_holder = new MFuncHolder<T>(instance, func);
	}
	
	// �֐��ݒ�
	void SetFunction(void (*func)(UCHAR))
	{
		delete m_holder;
		m_holder = new FuncHolder(func);
	}
	
	// �Ăяo��
	void Execute(UCHAR uc)
	{
		if(m_holder)
			m_holder->Execute(uc);
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
	
	// �����o�֐��p
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
			if(m_pMFunc)
				(m_instance.*m_pMFunc)(uc);
		}
		
	private:
		T&		  m_instance;
		void (T::*m_pMFunc)(UCHAR);
		
		MFuncHolder& operator=(const MFuncHolder &);
	};
	
	// �֐��p
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
			if(m_pFunc)
				(*m_pFunc)(uc);
		}
		
	private:
		void (*m_pFunc)(UCHAR);
	};
	
	HolderBase *		m_holder;
	
	// �ꉞ�R�s�[�֎~
	Caller& operator=(const Caller &);
	Caller(const Caller &);
};
