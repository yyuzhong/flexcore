/*
 * clock.hpp
 *
 *  Created on: Sep 21, 2015
 *      Author: ckielwein
 */

#ifndef SRC_CLOCK_CLOCK_HPP_
#define SRC_CLOCK_CLOCK_HPP_

#include <atomic>
#include <chrono>

namespace fc
{

namespace chrono
{

struct wall_clock
{
	/// system_wall_clock is just a forward to std::system_clock
	typedef std::chrono::system_clock system;
	/// steady_wall_clock is just a forward to std::steady_clock
	typedef std::chrono::steady_clock steady;
};


struct virtual_clock
{
	typedef std::chrono::system_clock::rep rep;
	typedef std::chrono::system_clock::period period;
	typedef std::chrono::steady_clock::time_point time_point;

	class master;
	class system
	{
	public:
		static constexpr bool is_steady = false;

		typedef virtual_clock::rep rep;
		typedef virtual_clock::period period;
		typedef virtual_clock::time_point time_point;
		typedef std::chrono::system_clock::duration duration;

		/**
		 * \brief returns current absolute simulation time
		 * \return A time point representing the current virtual time.
		 */
		static time_point now() noexcept;
		static std::time_t to_time_t( const time_point& t );
		static time_point from_time_t( std::time_t t );

		friend class master;
	private:

		void advance();
		void set_time(time_point r);

		static std::atomic<time_point> current_time;
	};

	/**
	 * \brief steady virtual clock for measuring time differences in simulation time
	 *
	 * Fulfills trivial_clock from std.
	 */
	struct steady
	{
		static constexpr bool is_steady = true;

		typedef virtual_clock::rep rep;
		typedef virtual_clock::period period;
		typedef virtual_clock::time_point time_point;
		typedef std::chrono::steady_clock::duration duration;

		/**
		 * \brief returns current relative simulation time
		 * \return A time point representing the current virtual time.
		 * \post if now is called twice with results t1 and t2, t2 >= t1 holds.
		 */
		static time_point now() noexcept;

	private:
		friend class master;

		void advance();

		static std::atomic<time_point> current_time;
	};

	class master
	{
	public:
		/**
		 * \brief advances clock by a single tick
		 *
		 * this method controls the flow of time of the virtual clock.
		 * The Scheduler calls advance during runtime of the program to advance the time.
		 */
		static void advance()
		{
			steady_clock.advance();
			system_clock.advance();
		}
		void set_time(time_point r)
		{
			system_clock.set_time(r);
			//do not set time of steady clock, as it has only relative timings.
		}

	private:
		static steady steady_clock;
		static system system_clock;
	};
};

} //namespace chrono
}  //namespace fc

#endif /* SRC_CLOCK_CLOCK_HPP_ */
