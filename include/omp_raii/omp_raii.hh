#pragma once
#ifndef OMP_RAII_HH_1A5VSVG6
#define OMP_RAII_HH_1A5VSVG6

#include <omp.h>
 
// RAII scoped lock (lifetime of the lock)
class omp_lock final {
	private:
		omp_lock_t m_lock;
	
	public:
		// RAII scoped access to an omp_lock
		class omp_lock_pass final {
			private:
				friend class omp_lock;
				
				omp_lock_t *m_lock_ptr;
				
				// constructor A; reference to lock to set (possibly blocking wait)
				omp_lock_pass( omp_lock_t &lock ) noexcept:
					m_lock_ptr { &lock }
				{
					omp_set_lock( m_lock_ptr );
				}
				
				// constructor B; pointer to lock that's already passed an omp_test_lock() in the outer class;
				// since it's already set by the test, there's no need to set it in the constructor
				omp_lock_pass( omp_lock_t *tested_lock_ptr ) noexcept:
					m_lock_ptr { tested_lock_ptr }
				{}
				
			public:
				~omp_lock_pass() noexcept {
					omp_unset_lock( m_lock_ptr );
				}
				
				omp_lock_pass(    omp_lock_pass const & ) = delete;
				omp_lock_pass(    omp_lock_pass      && ) = delete;
				auto & operator=( omp_lock_pass const & ) = delete;
				auto & operator=( omp_lock_pass      && ) = delete; 
		}; // end-of-class omp_lock_pass
		
		// essentially a std::optional-ish omp_lock_pass wrapper
		class omp_maybe_lock_pass final {
				omp_lock_pass *m_lock_pass_ptr;
			public:
				omp_maybe_lock_pass( omp_lock_pass *pass_ptr = nullptr ) noexcept:
					m_lock_pass_ptr { pass_ptr }
				{}
				
				omp_maybe_lock_pass( omp_maybe_lock_pass const & ) = delete;
				omp_maybe_lock_pass( omp_maybe_lock_pass      && ) = delete;
				auto & operator=(    omp_maybe_lock_pass const & ) = delete;
				auto & operator=(    omp_maybe_lock_pass      && ) = delete;
				
				~omp_maybe_pass() noexcept {
					if (m_lock_pass_ptr)
						delete m_lock_pass_ptr;
				}
				
				[[nodiscard]] bool has_value() const noexcept {
					return m_lock_pass_ptr != nullptr;
				}
				
				operator bool() const noexcept {
					return m_lock_pass_ptr != nullptr;
				}
		}; // end-of-class omp_maybe_lock_pass
		
		omp_lock() noexcept {
			omp_init_lock( &m_lock );
		}
		
		omp_lock(         omp_lock const & ) = delete;
		omp_lock(         omp_lock      && ) = delete;
		auto & operator=( omp_lock const & ) = delete;
		auto & operator=( omp_lock      && ) = delete; 
		
		// blocking wait for a scoped RAII pass
		[[nodiscard]] omp_lock_pass
		wait_for_pass() noexcept {
			return { m_lock };
		}
		
		// get a scoped RAII pass if available (no waiting)
		[[nodiscard]] omp_maybe_lock_pass
		request_pass() noexcept {
			if ( omp_test_lock( &m_lock ) )
				return { new omp_lock_pass(&m_lock) };
			else return {};
		}
		
		~omp_lock() noexcept {
			omp_destroy_lock( &m_lock );
		}
}; // end-of-class omp_lock
  
#endif // end-of-header-guard OMP_RAII_HH_1A5VSVG6
