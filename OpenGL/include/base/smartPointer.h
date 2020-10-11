//
//  SmartPointer.h
//  openGLTest
//
//  Created by xt on 18/6/10.
//  Copyright © 2018年 xt. All rights reserved.
//

#ifndef SmartPointer_h
#define SmartPointer_h

namespace base {

	template<class T> class SmartPointer
	{
		T *pT;
		void saveNew() { if (pT != 0) pT->newRef(); }
		void saveDel() { if (pT != 0) pT->delRef(); }

	public:
		SmartPointer(T *_pT = 0) : pT(_pT) { saveNew(); }
		SmartPointer(const SmartPointer<T>& _smartP) : pT(_smartP.pT) { saveNew(); }
		~SmartPointer() { saveDel(); }

		operator T*() const { return pT; }
		T& operator*() const { return *pT; }
		T* operator->() const { return pT; }
		T* addr() { return pT; }

		SmartPointer<T>& operator=(const SmartPointer<T>& smartP) {
			return operator=(smartP.pT);
		}

		SmartPointer<T>& operator=(T* _pT) {
			if (pT != _pT) {
				saveDel();
				pT = _pT;
				saveNew();
			}
			return (*this);
		}

		bool operator==(T*) const;
		bool operator!=(T*) const;
		bool operator==(const SmartPointer<T>&) const;
		bool operator!=(const SmartPointer<T>&) const;

		template<class T2> operator SmartPointer<T2>() { return SmartPointer<T2>(pT); }
	};

	template<class T>
	inline bool SmartPointer<T>::operator==(T *_pT) const {
		return (pT == _pT);
	}

	template<class T>
	inline bool SmartPointer<T>::operator!=(T *_pT) const {
		return !operator==(_pT);
	}

	template<class T>
	inline bool SmartPointer<T>::operator==(const SmartPointer& _pT) const {
		return operator==(_pT.pT);
	}

	template<class T>
	inline bool SmartPointer<T>::operator!= (const SmartPointer& _pT) const {
		return !operator==(_pT);
	}

}
#endif /* SmartPointer_h */
