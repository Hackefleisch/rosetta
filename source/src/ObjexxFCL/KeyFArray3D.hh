#ifndef INCLUDED_ObjexxFCL_KeyFArray3D_hh
#define INCLUDED_ObjexxFCL_KeyFArray3D_hh


// KeyFArray3D: Key-Access Fortran-Compatible 3D Array
//
// Project: Objexx Fortran Compatibility Library (ObjexxFCL)
//
// Version: 3.0.0
//
// Language: C++
//
// Copyright (c) 2000-2009 Objexx Engineering, Inc. All Rights Reserved.
// Use of this source code or any derivative of it is restricted by license.
// Licensing is available from Objexx Engineering, Inc.:  http://objexx.com  Objexx@objexx.com


// ObjexxFCL Headers
#include <ObjexxFCL/KeyFArray3D.fwd.hh>
#include <ObjexxFCL/FArray3.hh>
#include <ObjexxFCL/ObserverMulti.hh>
#include <ObjexxFCL/DynamicIndexRange.hh>
#include <ObjexxFCL/FArrayInitializer.hh>


namespace ObjexxFCL {


/// @brief KeyFArray3D: Key-Access Fortran-Compatible 3D Array
template< typename T >
class KeyFArray3D :
	public FArray3< T >,
	public ObserverMulti
{


private: // Types


	typedef  FArray3< T >  Super;
	typedef  internal::InitializerSentinel  InitializerSentinel;


private: // Friend


	template< typename > friend class KeyFArray3D;
	friend class FArray3D< T >;
	friend class FArray3P< T >;
	friend class FArray3A< T >;


public: // Types


	typedef  typename Super::Base  Base;
	typedef  typename Base::Section  Section;
	typedef  typename Super::IR  SIR;
	typedef  DynamicIndexRange  IR;

	// STL Style
	typedef  typename Base::value_type  value_type;
	typedef  typename Base::reference  reference;
	typedef  typename Base::const_reference  const_reference;
	typedef  typename Base::pointer  pointer;
	typedef  typename Base::const_pointer  const_pointer;
	typedef  typename Base::size_type  size_type;
	typedef  typename Base::difference_type  difference_type;

	// C++ Style
	typedef  typename Base::Value  Value;
	typedef  typename Base::Reference  Reference;
	typedef  typename Base::ConstReference  ConstReference;
	typedef  typename Base::Pointer  Pointer;
	typedef  typename Base::ConstPointer  ConstPointer;
	typedef  typename Base::Size  Size;
	typedef  typename Base::Difference  Difference;

	typedef  FArrayInitializer< T, ObjexxFCL::KeyFArray3D >  Initializer;
	typedef  typename Initializer::function_type  InitializerFunction;

	using Super::array_;
	using Super::array_size_;
	using Super::sarray_;
	using Super::shift_;
	using Super::shift_set;
	using Super::size_;
	using Super::size_of;
	using Super::s1_;
	using Super::s2_;


public: // Creation


	/// @brief Default Constructor
	inline
	KeyFArray3D()
	{
		insert_as_observer();
	}


	/// @brief Copy Constructor
	inline
	KeyFArray3D( KeyFArray3D const & a ) :
		Super( a ),
		ObserverMulti(),
		I1_( a.I1_ ),
		I2_( a.I2_ ),
		I3_( a.I3_ )
	{
		insert_as_observer();
	}


	/// @brief Copy Constructor Template
	template< typename U >
	inline
	explicit
	KeyFArray3D( KeyFArray3D< U > const & a ) :
		Super( a ),
		I1_( a.I1_ ),
		I2_( a.I2_ ),
		I3_( a.I3_ )
	{
		insert_as_observer();
	}


	/// @brief Super Constructor Template
	template< typename U >
	inline
	explicit
	KeyFArray3D( FArray3< U > const & a ) :
		Super( a ),
		I1_( a.I1() ),
		I2_( a.I2() ),
		I3_( a.I3() )
	{
		insert_as_observer();
	}


	/// @brief IndexRange Constructor
	inline
	KeyFArray3D( IR const & I1_a, IR const & I2_a, IR const & I3_a ) :
		Super( size_of( I1_a, I2_a, I3_a ) ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a )
	{
		setup_real();
		insert_as_observer();
	}


	/// @brief IndexRange + Initializer Value Constructor
	inline
	KeyFArray3D( IR const & I1_a, IR const & I2_a, IR const & I3_a, T const & t ) :
		Super( size_of( I1_a, I2_a, I3_a ), InitializerSentinel() ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a ),
		initializer_( t )
	{
		setup_real();
		initialize();
		insert_as_observer();
	}


	/// @brief IndexRange + Initializer Function Constructor
	inline
	KeyFArray3D( IR const & I1_a, IR const & I2_a, IR const & I3_a, InitializerFunction const & function_a ) :
		Super( size_of( I1_a, I2_a, I3_a ), InitializerSentinel() ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a ),
		initializer_( function_a )
	{
		setup_real();
		initialize();
		insert_as_observer();
	}


	/// @brief Super + IndexRange Constructor Template
	template< typename U >
	inline
	KeyFArray3D( FArray3< U > const & a, IR const & I1_a, IR const & I2_a, IR const & I3_a ) :
		Super( size_of( I1_a, I2_a, I3_a ) ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a )
	{
		setup_real();
		if ( dimensions_initialized() ) {
			if ( a.dimensions_initialized() ) { // Copy array data where overlap
				int const b1( std::max( I1_.l(), a.l1() ) ), e1( std::min( I1_.u(), a.u1() ) );
				int const b2( std::max( I2_.l(), a.l2() ) ), e2( std::min( I2_.u(), a.u2() ) );
				int const b3( std::max( I3_.l(), a.l3() ) ), e3( std::min( I3_.u(), a.u3() ) );
				for ( int i3 = b3; i3 <= e3; ++i3 ) {
					for ( int i2 = b2; i2 <= e2; ++i2 ) {
						for ( int i1 = b1; i1 <= e1; ++i1 ) {
							operator ()( i1, i2, i3 ) = T( a( i1, i2, i3 ) );
						}
					}
				}
			}
		}
		insert_as_observer();
	}


	/// @brief Super + IndexRange + Fill Value Constructor Template
	template< typename U >
	inline
	KeyFArray3D( FArray3< U > const & a, IR const & I1_a, IR const & I2_a, IR const & I3_a, T const & t ) :
		Super( size_of( I1_a, I2_a, I3_a ) ),
		I1_( I1_a ),
		I2_( I2_a ),
		I3_( I3_a )
	{
		setup_real();
		if ( dimensions_initialized() ) {
			(*this) = t; // Initialize array with fill value
			if ( a.dimensions_initialized() ) { // Copy array data where overlap
				int const b1( std::max( I1_.l(), a.l1() ) ), e1( std::min( I1_.u(), a.u1() ) );
				int const b2( std::max( I2_.l(), a.l2() ) ), e2( std::min( I2_.u(), a.u2() ) );
				int const b3( std::max( I3_.l(), a.l3() ) ), e3( std::min( I3_.u(), a.u3() ) );
				for ( int i3 = b3; i3 <= e3; ++i3 ) {
					for ( int i2 = b2; i2 <= e2; ++i2 ) {
						for ( int i1 = b1; i1 <= e1; ++i1 ) {
							operator ()( i1, i2, i3 ) = T( a( i1, i2, i3 ) );
						}
					}
				}
			}
		}
		insert_as_observer();
	}


	/// @brief Destructor
	inline
	~KeyFArray3D() override
	{}


public: // Assignment


	/// @brief Copy Assignment
	inline
	KeyFArray3D &
	operator =( KeyFArray3D const & a )
	{
		if ( this != &a ) {
			if ( ! equal_dimension( a ) ) dimension( a );
			Base::operator =( a );
		}
		return *this;
	}


	/// @brief Super Assignment
	inline
	KeyFArray3D &
	operator =( Super const & a )
	{
		if ( this != &a ) {
			if ( ! equal_dimension( a ) ) dimension( a );
			Base::operator =( a );
		}
		return *this;
	}


	/// @brief Super Assignment Template
	template< typename U >
	inline
	KeyFArray3D &
	operator =( FArray3< U > const & a )
	{
		if ( ! equal_dimension( a ) ) dimension( a );
		Base::operator =( a );
		return *this;
	}


	/// @brief += Array Template
	template< typename U >
	inline
	KeyFArray3D &
	operator +=( FArray3< U > const & a )
	{
		Super::operator +=( a );
		return *this;
	}


	/// @brief -= Array Template
	template< typename U >
	inline
	KeyFArray3D &
	operator -=( FArray3< U > const & a )
	{
		Super::operator -=( a );
		return *this;
	}


	/// @brief = Value
	inline
	KeyFArray3D &
	operator =( T const & t )
	{
		Super::operator =( t );
		return *this;
	}


	/// @brief += Value
	inline
	KeyFArray3D &
	operator +=( T const & t )
	{
		Super::operator +=( t );
		return *this;
	}


	/// @brief -= Value
	inline
	KeyFArray3D &
	operator -=( T const & t )
	{
		Super::operator -=( t );
		return *this;
	}


	/// @brief *= Value
	inline
	KeyFArray3D &
	operator *=( T const & t )
	{
		Super::operator *=( t );
		return *this;
	}


	/// @brief /= Value
	inline
	KeyFArray3D &
	operator /=( T const & t )
	{
		Super::operator /=( t );
		return *this;
	}


public: // Subscript


	/// @brief array( i1, i2, i3 ) const
	template< typename K1, typename K2, typename K3 >
	inline
	T const &
	operator ()( K1 const & i1, K2 const & i2, K3 const & i3 ) const
	{
		assert( ( I1_.contains( i1 ) ) && ( I2_.contains( i2 ) ) && ( I3_.contains( i3 ) ) );
		return sarray_[ ( ( ( i3 * s2_ ) + i2 ) * s1_ ) + i1 ];
	}


	/// @brief array( i1, i2, i3 )
	template< typename K1, typename K2, typename K3 >
	inline
	T &
	operator ()( K1 const & i1, K2 const & i2, K3 const & i3 )
	{
		assert( ( I1_.contains( i1 ) ) && ( I2_.contains( i2 ) ) && ( I3_.contains( i3 ) ) );
		return sarray_[ ( ( ( i3 * s2_ ) + i2 ) * s1_ ) + i1 ];
	}


	/// @brief Section Starting at array( i1, i2, i3 )
	template< typename K1, typename K2, typename K3 >
	inline
	Section const
	a( K1 const & i1, K2 const & i2, K3 const & i3 ) const
	{
		assert( ( I1_.contains( i1 ) ) && ( I2_.contains( i2 ) ) && ( I3_.contains( i3 ) ) );
		size_type const offset( ( ( ( ( i3 * s2_ ) + i2 ) * s1_ ) + i1 ) - shift_ );
		return Section( array_size_ - offset, array_ + offset );
	}


	/// @brief Linear Index
	template< typename K1, typename K2, typename K3 >
	inline
	size_type
	index( K1 const & i1, K2 const & i2, K3 const & i3 ) const
	{
		assert( ( I1_.initialized() ) && ( I2_.initialized() ) && ( I3_.initialized() ) );
		return ( ( ( ( ( i3 * s2_ ) + i2 ) * s1_ ) + i1 ) - shift_ );
	}


	/// @brief array[ i ] const: Linear Subscript
	inline
	T const &
	operator []( size_type const i ) const
	{
		assert( i < size_ );
		return array_[ i ];
	}


	/// @brief array[ i ]: Linear Subscript
	inline
	T &
	operator []( size_type const i )
	{
		assert( i < size_ );
		return array_[ i ];
	}


public: // Predicate


	/// @brief Dimensions Initialized?
	inline
	bool
	dimensions_initialized() const
	{
		return ( ( I1_.initialized() ) && ( I2_.initialized() ) && ( I3_.initialized() ) );
	}


	/// @brief Contains Indexed Element?
	template< typename K1, typename K2, typename K3 >
	inline
	bool
	contains( K1 const & i1, K2 const & i2, K3 const & i3 ) const
	{
		return ( ( I1_.contains( i1 ) ) && ( I2_.contains( i2 ) ) && ( I3_.contains( i3 ) ) );
	}


	/// @brief Initializer Active?
	inline
	bool
	initializer_active() const
	{
		return initializer_.is_active();
	}


public: // Inspector


	/// @brief IndexRange of Dimension 1
	inline
	IR const &
	I1() const
	{
		return I1_;
	}


	/// @brief Lower Index of Dimension 1
	inline
	int
	l1() const
	{
		return I1_.l();
	}


	/// @brief Upper Index of Dimension 1
	inline
	int
	u1() const
	{
		return I1_.u();
	}


	/// @brief IndexRange of Dimension 2
	inline
	IR const &
	I2() const
	{
		return I2_;
	}


	/// @brief Lower Index of Dimension 2
	inline
	int
	l2() const
	{
		return I2_.l();
	}


	/// @brief Upper Index of Dimension 2
	inline
	int
	u2() const
	{
		return I2_.u();
	}


	/// @brief IndexRange of Dimension 3
	inline
	IR const &
	I3() const
	{
		return I3_;
	}


	/// @brief Lower Index of Dimension 3
	inline
	int
	l3() const
	{
		return I3_.l();
	}


	/// @brief Upper Index of Dimension 3
	inline
	int
	u3() const
	{
		return I3_.u();
	}


	/// @brief Size of Dimension 3
	inline
	size_type
	size3() const
	{
		return I3_.size();
	}


public: // Modifier


	/// @brief Clear
	inline
	KeyFArray3D &
	clear()
	{
		Super::clear();
		I1_.clear_no_notify();
		I2_.clear_no_notify();
		I3_.clear_no_notify();
		initializer_.clear();
		notify();
		return *this;
	}


	/// @brief Dimension by IndexRange
	inline
	KeyFArray3D &
	dimension( IR const & I1_a, IR const & I2_a, IR const & I3_a )
	{
		initializer_.clear();
		I1_.assign_no_notify( I1_a );
		I2_.assign_no_notify( I2_a );
		I3_.assign_no_notify( I3_a );
		dimension_real();
		notify();
		return *this;
	}


	/// @brief Dimension by IndexRange + Initializer Value
	inline
	KeyFArray3D &
	dimension( IR const & I1_a, IR const & I2_a, IR const & I3_a, T const & t )
	{
		initializer_ = t;
		I1_.assign_no_notify( I1_a );
		I2_.assign_no_notify( I2_a );
		I3_.assign_no_notify( I3_a );
		dimension_real();
		initialize();
		notify();
		return *this;
	}


	/// @brief Dimension by IndexRange + Initializer Function
	inline
	KeyFArray3D &
	dimension( IR const & I1_a, IR const & I2_a, IR const & I3_a, InitializerFunction const & function_a )
	{
		initializer_ = function_a;
		I1_.assign_no_notify( I1_a );
		I2_.assign_no_notify( I2_a );
		I3_.assign_no_notify( I3_a );
		dimension_real();
		initialize();
		notify();
		return *this;
	}


	/// @brief Dimension by Array Template
	template< typename U >
	inline
	KeyFArray3D &
	dimension( FArray3< U > const & a )
	{
		initializer_.clear();
		I1_.assign_no_notify( a.I1() );
		I2_.assign_no_notify( a.I2() );
		I3_.assign_no_notify( a.I3() );
		dimension_real();
		notify();
		return *this;
	}


	/// @brief Dimension by Array + Initializer Value Template
	template< typename U >
	inline
	KeyFArray3D &
	dimension( FArray3< U > const & a, T const & t )
	{
		initializer_ = t;
		I1_.assign_no_notify( a.I1() );
		I2_.assign_no_notify( a.I2() );
		I3_.assign_no_notify( a.I3() );
		dimension_real();
		initialize();
		notify();
		return *this;
	}


	/// @brief Dimension by Array + Initializer Function Template
	template< typename U >
	inline
	KeyFArray3D &
	dimension( FArray3< U > const & a, InitializerFunction const & function_a )
	{
		initializer_ = function_a;
		I1_.assign_no_notify( a.I1() );
		I2_.assign_no_notify( a.I2() );
		I3_.assign_no_notify( a.I3() );
		dimension_real();
		initialize();
		notify();
		return *this;
	}


	/// @brief Data-Preserving Redimension by IndexRange
	inline
	KeyFArray3D &
	redimension( IR const & I1_a, IR const & I2_a, IR const & I3_a )
	{
		KeyFArray3D( *this, I1_a, I2_a, I3_a ).swap( *this );
		return *this;
	}


	/// @brief Data-Preserving Redimension by IndexRange + Fill Value
	inline
	KeyFArray3D &
	redimension( IR const & I1_a, IR const & I2_a, IR const & I3_a, T const & t )
	{
		KeyFArray3D( *this, I1_a, I2_a, I3_a, t ).swap( *this );
		return *this;
	}


	/// @brief Data-Preserving Redimension by Array Template
	template< typename U >
	inline
	KeyFArray3D &
	redimension( FArray3< U > const & a )
	{
		KeyFArray3D( *this, a.I1(), a.I2(), a.I3() ).swap( *this );
		return *this;
	}


	/// @brief Data-Preserving Redimension by Array + Fill Value Template
	template< typename U >
	inline
	KeyFArray3D &
	redimension( FArray3< U > const & a, T const & t )
	{
		KeyFArray3D( *this, a.I1(), a.I2(), a.I3(), t ).swap( *this );
		return *this;
	}


	/// @brief Set Initializer Value
	inline
	KeyFArray3D &
	initializer( T const & t )
	{
		initializer_ = t;
		return *this;
	}


	/// @brief Set Initializer Function
	inline
	KeyFArray3D &
	initializer( InitializerFunction const & function_a )
	{
		initializer_ = function_a;
		return *this;
	}


	/// @brief Clear Initializer
	inline
	KeyFArray3D &
	initializer_clear()
	{
		initializer_.clear();
		return *this;
	}


	/// @brief Initialize
	inline
	KeyFArray3D &
	initialize()
	{
		if ( ( initializer_.is_active() ) && ( dimensions_initialized() ) ) {
			if ( initializer_.is_value() ) {
				(*this) = initializer_.value();
			} else if ( initializer_.is_function() ) {
				initializer_.function()( *this );
			}
		}
		return *this;
	}


	/// @brief Swap
	inline
	KeyFArray3D &
	swap( KeyFArray3D & v )
	{
		swap3DB( v );
		I1_.swap_no_notify( v.I1_ );
		I2_.swap_no_notify( v.I2_ );
		I3_.swap_no_notify( v.I3_ );
		std::swap( initializer_, v.initializer_ );
		notify(); // So proxy FArrays can reattach
		v.notify(); // So proxy FArrays can reattach
		return *this;
	}


public: // Observer Modifier


	/// @brief Update
	inline
	void
	update() override
	{
		dimension_real();
		initialize();
	}


	/// @brief Update for Destruction of a Subject
	inline
	void
	destructed( Subject const & ) override
	{}


public: // Friend


	/// @brief Swap
	friend
	inline
	void
	swap( KeyFArray3D & a, KeyFArray3D & b )
	{
		a.swap( b );
	}


public: // Generator


	/// @brief -Array
	friend
	inline
	KeyFArray3D
	operator -( KeyFArray3D const & a )
	{
		KeyFArray3D r( a );
		r *= T( -1 );
		return r;
	}


	/// @brief Array + Array
	friend
	inline
	KeyFArray3D
	operator +( KeyFArray3D const & a, KeyFArray3D const & b )
	{
		KeyFArray3D r( a );
		r += b;
		return r;
	}


	/// @brief Array - Array
	friend
	inline
	KeyFArray3D
	operator -( KeyFArray3D const & a, KeyFArray3D const & b )
	{
		KeyFArray3D r( a );
		r -= b;
		return r;
	}


	/// @brief Array + Value
	friend
	inline
	KeyFArray3D
	operator +( KeyFArray3D const & a, T const & t )
	{
		KeyFArray3D r( a );
		r += t;
		return r;
	}


	/// @brief Value + Array
	friend
	inline
	KeyFArray3D
	operator +( T const & t, KeyFArray3D const & a )
	{
		KeyFArray3D r( a );
		r += t;
		return r;
	}


	/// @brief Array - Value
	friend
	inline
	KeyFArray3D
	operator -( KeyFArray3D const & a, T const & t )
	{
		KeyFArray3D r( a );
		r -= t;
		return r;
	}


	/// @brief Value - Array
	friend
	inline
	KeyFArray3D
	operator -( T const & t, KeyFArray3D const & a )
	{
		KeyFArray3D r( a );
		r *= T( -1 );
		r += t;
		return r;
	}


	/// @brief Array * Value
	friend
	inline
	KeyFArray3D
	operator *( KeyFArray3D const & a, T const & t )
	{
		KeyFArray3D r( a );
		r *= t;
		return r;
	}


	/// @brief Value * Array
	friend
	inline
	KeyFArray3D
	operator *( T const & t, KeyFArray3D const & a )
	{
		KeyFArray3D r( a );
		r *= t;
		return r;
	}


	/// @brief Array / Value
	friend
	inline
	KeyFArray3D
	operator /( KeyFArray3D const & a, T const & t )
	{
		KeyFArray3D r( a );
		r /= t;
		return r;
	}


protected: // Functions


	/// @brief Dimension by IndexRanges
	inline
	void
	dimension_assign( SIR const & I1_a, SIR const & I2_a, SIR const & I3_a )
	{
		initializer_.clear();
		I1_.assign_no_notify( I1_a );
		I2_.assign_no_notify( I2_a );
		I3_.assign_no_notify( I3_a );
		dimension_real();
		notify();
	}


private: // Functions


	/// @brief Setup for IndexRange Constructor
	inline
	void
	setup_real()
	{
		s1_ = I1_.size();
		s2_ = I2_.size();
		if ( dimensions_initialized() ) {
			shift_set( ( ( ( I3_.lz() * s2_ ) + I2_.lz() ) * s1_ ) + I1_.lz() );
		} else {
			shift_set( 0 );
		}
	}


	/// @brief Dimension by Current IndexRanges
	inline
	void
	dimension_real()
	{
		s1_ = I1_.size();
		s2_ = I2_.size();
		if ( dimensions_initialized() ) {
			resize( size_of( s1_, s2_, I3_.size() ) );
			shift_set( ( ( ( I3_.lz() * s2_ ) + I2_.lz() ) * s1_ ) + I1_.lz() );
#ifdef OBJEXXFCL_FARRAY_SIZE_REPORT
			size_report();
#endif // OBJEXXFCL_FARRAY_SIZE_REPORT
		} else {
			Base::clear();
		}
	}


	/// @brief Insert as Observer of the IndexRanges
	inline
	void
	insert_as_observer()
	{
		I1_.insert_observer( *this );
		I2_.insert_observer( *this );
		I3_.insert_observer( *this );
#ifdef OBJEXXFCL_FARRAY_SIZE_REPORT
		size_report();
#endif // OBJEXXFCL_FARRAY_SIZE_REPORT
	}

	/* // unused private
	/// @brief Remove as Observer of the IndexRanges
	inline
	void
	remove_as_observer()
	{
		I1_.remove_observer( *this );
		I2_.remove_observer( *this );
		I3_.remove_observer( *this );
	}*/


#ifdef OBJEXXFCL_FARRAY_SIZE_REPORT
	/// @brief Report size if at least value defined for OBJEXXFCL_FARRAY_SIZE_REPORT
	/// @note  Size is based on sizeof( T ) so T-controlled heap memory is not counted
	inline
	void
	size_report() const
	{
		if ( size_ * sizeof( T ) >= OBJEXXFCL_FARRAY_SIZE_REPORT ) {
			std::cout << "  Index ranges: " << I1_ << ' ' << I2_ << ' ' << I3_ << std::endl;
		}
	}
#endif // OBJEXXFCL_FARRAY_SIZE_REPORT


private: // Data


	/// @brief Dimension 1 index range
	IR I1_;

	/// @brief Dimension 2 index range
	IR I2_;

	/// @brief Dimension 3 index range
	IR I3_;

	/// @brief Array initializer
	Initializer initializer_;


}; // KeyFArray3D


/// @brief Swap
template< typename T >
void
swap( KeyFArray3D< T > & a, KeyFArray3D< T > & b );


/// @brief -Array
template< typename T >
KeyFArray3D< T >
operator -( KeyFArray3D< T > const & a );


/// @brief Array + Array
template< typename T >
KeyFArray3D< T >
operator +( KeyFArray3D< T > const & a, KeyFArray3D< T > const & b );


/// @brief Array - Array
template< typename T >
KeyFArray3D< T >
operator -( KeyFArray3D< T > const & a, KeyFArray3D< T > const & b );


/// @brief Array + Value
template< typename T >
KeyFArray3D< T >
operator +( KeyFArray3D< T > const & a, T const & t );


/// @brief Value + Array
template< typename T >
KeyFArray3D< T >
operator +( T const & t, KeyFArray3D< T > const & a );


/// @brief Array - Value
template< typename T >
KeyFArray3D< T >
operator -( KeyFArray3D< T > const & a, T const & t );


/// @brief Value - Array
template< typename T >
KeyFArray3D< T >
operator -( T const & t, KeyFArray3D< T > const & a );


/// @brief Array * Value
template< typename T >
KeyFArray3D< T >
operator *( KeyFArray3D< T > const & a, T const & t );


/// @brief Value * Array
template< typename T >
KeyFArray3D< T >
operator *( T const & t, KeyFArray3D< T > const & a );


/// @brief Array / Value
template< typename T >
KeyFArray3D< T >
operator /( KeyFArray3D< T > const & a, T const & t );


} // namespace ObjexxFCL


#ifndef NO_STD_SWAP_OVERLOADS


// std::swap Overloads for Efficiency
//
// Technically you cannot add template functions overloads to namespace std
// but this works with most compilers and makes it much faster if someone uses
// std::swap instead of swap or ObjexxFCL::swap.  The legal alternative would be
// to add specializations of swap for each anticipated instantiation.


namespace std {


/// @brief std::swap( KeyFArray3D, KeyFArray3D )
template< typename T >
inline
void
swap( ObjexxFCL::KeyFArray3D< T > & a, ObjexxFCL::KeyFArray3D< T > & b )
{
	a.swap( b );
}


} // namespace std


#endif // NO_STD_SWAP_OVERLOADS


#endif // INCLUDED_ObjexxFCL_KeyFArray3D_HH
