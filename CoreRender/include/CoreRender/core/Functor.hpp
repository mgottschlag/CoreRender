/*
Copyright (C) 2009, Mathias Gottschlag

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _CORERENDER_CORE_FUNCTOR_HPP_INCLUDED_
#define _CORERENDER_CORE_FUNCTOR_HPP_INCLUDED_

namespace cr
{
namespace core
{
	/**
	 * Object which can be called like a function with no parameters.
	 * 
	 * This can be used for callbacks.
	 */
	class Functor
	{
		public:
			/**
			 * Destructor.
			 */
			virtual ~Functor()
			{
			}

			/**
			 * Calls the functor.
			 */
			virtual void call() = 0;
		private:
	};

	/**
	 * Function pointer to a non-static member function.
	 */
	template<class T> class ClassFunctor : public Functor
	{
		public:
			/**
			 * Constructor.
			 * 
			 * @param instance Will be passed as "this" when the function is
			 * called.
			 * @param function Non-static member function.
			 */
			ClassFunctor(T *instance, void (T::*function)(void))
				: instance(instance), function(function)
			{
			}

			virtual void call()
			{
				(instance->*function)();
			}
		private:
			T *instance;
			void (T::*function)(void);
	};

	/**
	 * Object which can be called like a function with one parameter.
	 * 
	 * This can be used for callbacks.
	 */
	template<class P1> class Functor1
	{
		public:
			virtual ~Functor1()
			{
			}

			/**
			 * Calls the functor.
			 * 
			 * @param p1 First function parameter.
			 */
			virtual void call(P1 p1) = 0;
		private:
	};

	/**
	 * Function pointer to a non-static member function with one parameter.
	 */
	template<class T, class P1> class ClassFunctor1 : public Functor1<P1>
	{
		public:
			/**
			 * Constructor.
			 * 
			 * @param instance Will be passed as "this" when the function is
			 * called.
			 * @param function Non-static member function.
			 */
			ClassFunctor1(T *instance, void (T::*function)(P1))
				: instance(instance), function(function)
			{
			}

			virtual void call(P1 p1)
			{
				(instance->*function)(p1);
			}
		private:
			T *instance;
			void (T::*function)(P1);
	};

	template<class P1, class P2> class Functor2
	{
		public:
			virtual ~Functor2()
			{
			}

			virtual void call(P1 p1, P2 p2) = 0;
		private:
	};

	template<class T, class P1, class P2> class ClassFunctor2 : public Functor2<P1, P2>
	{
		public:
			ClassFunctor2(T *instance, void (T::*function)(P1, P2))
				: instance(instance), function(function)
			{
			}

			virtual void call(P1 p1, P2 p2)
			{
				(instance->*function)(p1, p2);
			}
		private:
			T *instance;
			void (T::*function)(P1, P2);
	};
}
}

#endif
