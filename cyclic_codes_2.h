#ifndef CYCLIC_CODE_H
#define CYCLIC_CODE_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <climits>

using namespace std;

/**
 * A cyclic code class
 * @author Jared Allen
 * @version 14 December 2018
 */
class CyclicCode
{
public:
  /**
   * Constructor specifying generator and parity check
   * matrices
   * @param generator the generator matrix
   * @param parity_check the parity check matrix
   * @param code_length the length of the code
   */
  CyclicCode( vector< uint > generator, vector< uint > parity_check,
              uint code_length );

  /**
   * Return the generator matrix
   * @return the generator matrix
   */
  vector< uint > get_generator() const;

  /**
   * Return the code length
   */
  uint get_code_length() const;

  /**
   * Return the code words
   */
  vector< uint > get_code_words() const;

  /**
   * Print the code words
   */
  void print_words() const;

  /**
   * Print generator matrix
   */
  void print_generator() const;

  /**
   * Print parity check matrix
   */
  void print_parity_check() const;

  /**
   * determine if the word is part of the code
   * @param word the word to be checked 
   * @return if it is a word or not
   */
  bool is_code_word( uint word ) const;

  /**
   * determine the original word sent from the received word
   * @param word the word to be decoded
   * @return the nearest neighbor to the received word
   */
  uint decode_word( uint received_word ) const;

  /**
   * encode the word with this linear code.
   * @param word the word to be encoded
   * @return the encoded word
   */
  uint encode_word( uint word ) const;

private:

  /**
   * decodes using nearest neighbor decoding
   * @param word the word to be decoded
   * @return the nearest code word
   */
  uint nearest_neighbor( uint word ) const;

  /**
   * determines the burst length for a syndrome
   * @param syndrome the syndrome 
   * @param syndrome length the length of the syndrome
   */
  uint get_burst_length( uint syndrome, uint syndrome_length ) const;

  /**
   * computes syndromes for a received word
   * @param received_word
   * @return a list of syndromes
   */
  vector< uint > get_syndromes( uint received_word ) const;

  /**
   * computes all cyclic shifts for a codeword
   * @param word the word to shift
   * @return a list with all the shifted words
   */
  vector< uint > get_cyclic_shifts( uint word,
                                    uint word_length ) const;

  /**
   * determines the hamming distance between two words
   * @param first_word the first word
   * @param second_word the second word
   * @return the hamming distance between the two
   */
  uint hamming_distance( uint first_word, uint second_word ) const;
    
  /**
   * print the bitwise representation of a codeword
   * @param code_word the word to be printed
   */
  void print_word_bitwise( uint code_word ) const;
  
  /**
   * Implementation of matrix printing
   * @param matrix the matrix to be printed
   * @param code_length the length of the code
   */
  void print_matrix( vector< uint > matrix, uint code_length ) const;

  /**
   * determine the transpose of a matrix
   * @param matrix the matrix to be transposed
   * @param code_length the length of the code
   */
  vector< uint > get_transpose( vector< uint > matrix,
                                uint code_length ) const;

  /**
   * return a power of a given base
   * @param base the base
   * @param exponent the exponent
   * @return the result
   */
  uint find_power( uint base, uint exponent ) const;
  
  vector< uint > generator;
  vector< uint > parity_check;
  vector< uint > code_words;
  uint code_length;
  uint min_distance;
  uint max_burst_length;
};

CyclicCode::CyclicCode( vector< uint > param_generator,
                        vector< uint > param_parity_check,
                        uint param_code_length )
: generator( param_generator ), parity_check( param_parity_check ),
  code_length( param_code_length )
{
  //determine code words
  for( uint i = 0; i < find_power( 2, code_length ); i++ )
  {
    if( is_code_word( i ) )
    {
      code_words.push_back( i );
    }
  }

  //determine minimum distance of the code
  uint distance = UINT_MAX;
  for( uint word : code_words )
  {
    if( word > 0 )
    {
      uint word_weight = hamming_distance( 0, word );
      if( word_weight < distance )
      {
        distance = word_weight;
      }
    }
  }
  min_distance = distance;

  //determine maximum burst length
  max_burst_length = 3;
  
}

vector< uint > CyclicCode::get_code_words() const
{
  return code_words;
}

vector< uint > CyclicCode::get_generator() const
{
  return generator;
}

uint CyclicCode::get_code_length() const
{
  return code_length;
}

void CyclicCode::print_matrix( vector< uint > matrix,
                               uint code_length ) const
{
  //find and print the bitwise representation of matrix
  for( uint i = 0; i < matrix.size(); i++ )
  {
    uint this_code_word = matrix.at( i );
    vector< uint > code_word_bitwise;
    for( uint j = 0; j < code_length; j++ )
    {
      uint this_bit = ( this_code_word >> j ) & 1;
      code_word_bitwise.push_back( this_bit );
    }

    for( uint j = code_length - 1; j < UINT_MAX; j-- )
    {
      cout << code_word_bitwise.at( j );
    }
    cout << endl;
  }
  cout << endl;
}

void CyclicCode::print_word_bitwise( uint word ) const
{
  //find and print bitwise representation of codeword
  vector< uint > code_word_bitwise;
  for( uint j = 0; j < code_length; j++ )
  {
    uint this_bit = ( word >> j ) & 1;
    code_word_bitwise.push_back( this_bit );
  }

  for( uint i = code_length - 1; i < UINT_MAX; i-- )
    {
      cout << code_word_bitwise.at( i );
    }
    cout << endl;
}

void CyclicCode::print_generator() const
{
  cout << "the generator matrix" << endl;
  print_matrix( generator, code_length );
}

void CyclicCode::print_parity_check() const
{
  cout << "the parity check matrix" << endl;
  print_matrix( parity_check, code_length );
}

void CyclicCode::print_words() const
{
  cout << "The code words are: " << endl;
  for( uint i = 0; i < code_words.size(); i++ )
  {
    cout << code_words.at( i ) << " ";
    print_word_bitwise( code_words.at( i ) );
  }
  cout << endl;
  cout << "num code words: " << code_words.size() << endl;
}

bool CyclicCode::is_code_word( uint word ) const
{
  //determine the product between the parity
  //check matrix transpose and the word
  vector< uint > vector_product;
  for( uint col = 0; col < parity_check.size(); col++ )
  {
    //determine the dot product between each column of the
    //parity check matrix transpose and the word
    uint dot_product = 0;
    uint this_term = 0;
    for( uint row = 0; row < code_length; row++ )
    {
      uint word_coordinate = ( word >> row ) & 1;
      uint basis_vector_coordinate = ( parity_check.at( col )
                                       >> row ) & 1;
      
      this_term = word_coordinate & basis_vector_coordinate;
      dot_product = dot_product ^ this_term;
    }
    vector_product.push_back( dot_product );
  }

  //determine if the product is the zero vector
  bool is_word = true;
  for( uint this_word = 0; this_word < vector_product.size();
       this_word++ )
  {
    if( vector_product.at( this_word ) != 0 )
    {
      is_word = false;
    }
  }
  return is_word;
}

vector< uint > CyclicCode::get_transpose( vector< uint > matrix,
                               uint code_length ) const
{
  vector< uint > matrix_transpose;
  uint new_code_length = 0;

  //initialize the transpose matrix
  for( uint col = code_length; col != 0; col-- )
  {
    matrix_transpose.push_back( 0 );
  }

  //establish the new code length
  uint num_code_words = matrix.size();

  //add the appropriate values to create the
  //transpose matrix and return it
  for( uint old_row = 0; old_row < matrix.size(); old_row++ )
  {
    new_code_length++;
    uint this_old_row = matrix.at( old_row );
    for( uint col = code_length - 1; col != UINT_MAX; col-- )
    {
      uint new_row = code_length - col - 1;
      uint place_holder = ( this_old_row >> col ) & 1;
      uint place_value = num_code_words - old_row - 1;
      if( place_holder == 1 )
      {
        matrix_transpose.at( new_row ) +=
          find_power( 2, place_value );
      }
    }
  }

  return matrix_transpose;
}

uint CyclicCode::find_power( uint base, uint exponent ) const
{
  if( exponent == 0 )
  {
    return 1;
  }
  else
  {
    uint result = 1;
    for( uint i = 0; i < exponent; i++ )
    {
      result *= base;
    }
    return result;
  }
}

uint CyclicCode::encode_word( uint word ) const
{
  //determine the product of the word with the
  //generator matrix:

  //find the sum of the rows corresponding to the nonzero
  //coordinates of the word to be encoded
  uint encoded_word = 0;
  for( uint place_value = 0; place_value < generator.size();
       place_value++ )
  {
    if( ( ( word >> place_value ) & 1 ) == 1 )
    {
      encoded_word ^= generator.at( generator.size()
                                    - 1 - place_value );
    }
    
  }
  return encoded_word;
}

uint CyclicCode::decode_word( uint received_word ) const
{

  /*
  //first compute x^i * w(x) for 0 < i < n
  vector< uint > received_cyclic_shifts =
    get_cyclic_shifts( received_word, code_length );
  */

  vector< uint > cyclic_shifts =
    get_cyclic_shifts( received_word, code_length );

  vector< uint > received_cyclic_shifts = { cyclic_shifts.at( 0 ) };
  
  for( uint i = cyclic_shifts.size() - 1; i > 0; i-- )
  {
    received_cyclic_shifts.push_back( cyclic_shifts.at( i ) );
  }

  //next find corresponding syndromes
  vector< uint > syndromes = get_syndromes( received_word );
  
  //TESTS
  /*
  cout << "min distance : " << min_distance << endl;
  cout << "received word: ";
  print_word_bitwise( received_word );
  cout << "received word shifted cyclically: " << endl;
  for( uint word : received_cyclic_shifts )
  {
    print_word_bitwise( word );
  }
  cout << endl;

  cout << "the syndromes: " << endl;
  print_matrix( syndromes, parity_check.size() );

  cout << "the parity check matrix:" << endl;
  print_matrix( parity_check, code_length );
  */
  //END TESTS

  

  //determine burst length of each syndrome
  vector< uint > syndrome_burst_lengths;
  for( uint k = 0; k < syndromes.size(); k++ )
  {
    syndrome_burst_lengths.push_back( get_burst_length(
                                        syndromes.at( k ),
                                        parity_check.size() ) );
  }

  //TESTING
/*
  cout << "received word: ";
  print_word_bitwise( received_word );
  cout << endl;
  for( uint i = 0; i < syndromes.size(); i++  )
  {
    cout << syndrome_burst_lengths.at( i ) << endl;
    print_word_bitwise( syndromes.at( i ) );
    cout << endl;
  }
  cout << endl;
*/
  //TESTING

  //determine a syndrome that is a burst of length
  //desired burst length
  bool found_syndrome = false;
  uint light_syndrome = 0;
  uint light_syndrome_pos = 0;
  uint which_syndrome = 0;
  uint desired_burst_length = max_burst_length;
  while( desired_burst_length != UINT_MAX )
  {
    while( !found_syndrome && which_syndrome < syndromes.size() )
    {
      uint syndrome_burst =
        syndrome_burst_lengths.at( which_syndrome );
      if( syndrome_burst == desired_burst_length )
      {
        found_syndrome = true;
        light_syndrome = syndromes.at( which_syndrome );
        light_syndrome_pos = which_syndrome;
      }
      which_syndrome++;
    }
    which_syndrome = 0;
    desired_burst_length--;
  }

  //if no syndrome meets requirements, return original word
  //as syndrome decoding fails
  uint decoded_word = 0;
  if( !found_syndrome )
  {
    cout << "no syndrome found." << endl;
    cout << "Word ";
    print_word_bitwise( received_word );
    cout << " --failed to decode." << endl;
    cout << endl;
    return received_word;
  }

  //shift syndrome to align with proper degree term in polynomial
  //syndromes align left not right
  light_syndrome =
    light_syndrome << ( code_length - parity_check.size() );
  uint syndrome_length = parity_check.size();
  uint shift_amount = code_length - light_syndrome_pos;

  //construct shifted syndrome
  uint shifted_syndrome = 0;
  uint temp = light_syndrome;
  bool bit_wrap = false;
  for( uint i = 1; i <= shift_amount; i++ )
  {
    if( ( light_syndrome & 1 ) == 1 )
    {
      bit_wrap = true;
    }
    
    light_syndrome = light_syndrome >> 1;
    if( bit_wrap )
    {
      light_syndrome += find_power( 2, code_length - 1 );
    }
    bit_wrap = false;
  }

  shifted_syndrome = light_syndrome;
  light_syndrome = temp;

  decoded_word = received_word ^ shifted_syndrome;

  /* TESTING */

  if( !is_code_word( decoded_word ) )
  {
    cout << "received word shifted cyclically: " << endl;
    for( uint word : received_cyclic_shifts )
    {
      print_word_bitwise( word );
    }
    cout << endl;
  
    cout << "the corresponding syndromes: " << endl;
    print_matrix( syndromes, parity_check.size() );
    
    cout << "min distance : " << min_distance << endl;
    cout << "which syndrome: " << light_syndrome_pos << endl;
    cout << "light syndrome: ";
    print_word_bitwise( light_syndrome );
    cout << "shift syndrome: ";
    print_word_bitwise( shifted_syndrome );
    cout << "shift amount: " << shift_amount << endl;
    cout << "decoded word: ";
    print_word_bitwise( decoded_word );
  }
  if( is_code_word( decoded_word ) )
  {
    cout << "Word ";
    print_word_bitwise( received_word );
    cout << " --decoded successfully." << endl;
    cout << endl;
  }
  else
  {
    cout << "Word ";
    print_word_bitwise( received_word );
    cout << " --failed to decode." << endl;
    cout << endl;
  }

    
  /* END TESTING */
  return decoded_word;
  
}

uint CyclicCode::hamming_distance( uint first_word,
                                   uint second_word ) const
{
  uint places_differ = first_word ^ second_word;
  uint hamming_distance = 0;
  for( uint place_value = 0; place_value < code_length;
       place_value++ )
  {
    hamming_distance += ( ( places_differ >> place_value ) & 1 );
  }
  return hamming_distance;
}

vector< uint > CyclicCode::get_cyclic_shifts(
  uint word, uint word_length ) const
{
  
  vector< uint > bit_place_values;
  for( uint i = 0; i < word_length; i++ )
  {
    if( ( ( word >> i ) & 1 ) == 1 )
    {
      bit_place_values.push_back( i );
    }
  }
  
  vector< uint > cyclic_shifts = { word };
  for( uint i = 1; i < word_length; i++ )
  {
    for( uint j = 0; j < bit_place_values.size(); j++ )
    {
      bit_place_values.at( j ) =
        ( bit_place_values.at( j ) + 1 ) % ( word_length );
    }

    //create the corresponding word and add it to the list
    uint cyclic_shift = 0;
    for( uint place_value : bit_place_values )
    {
      cyclic_shift += find_power( 2, place_value );
    }
    
    cyclic_shifts.push_back( cyclic_shift );
  }
  return cyclic_shifts;
}

vector< uint > CyclicCode::get_syndromes( uint received_word ) const
{
  //determine cyclic shifts
  vector< uint > cyclic_shifts =
    get_cyclic_shifts( received_word, code_length );

  //reorder to represent multiplication by x^i for 0 <= i < n
  vector< uint > received_cyclic_shifts = { cyclic_shifts.at( 0 ) };
  
  for( uint i = cyclic_shifts.size() - 1; i > 0; i-- )
  {
    received_cyclic_shifts.push_back( cyclic_shifts.at( i ) );
  }

  vector< uint > syndromes;
  vector< uint > parity_transpose = get_transpose( parity_check,
                                                   code_length );
  for( uint i = 0; i < received_cyclic_shifts.size(); i++ )
  {
    uint syndrome = 0;
    for( uint place_value = 0; place_value < parity_transpose.size();
         place_value++ )
    {
      if( ( ( received_cyclic_shifts.at( i )
              >> place_value ) & 1 ) == 1 )
      {
        syndrome ^= parity_transpose.at( parity_transpose.size()
                                         - 1 - place_value );
      }
    }
    syndromes.push_back( syndrome );
  }
  return syndromes;
}

uint CyclicCode::get_burst_length( uint syndrome,
                                 uint syndrome_length ) const
{
  vector< uint > cyclic_shifts =
    get_cyclic_shifts( syndrome, syndrome_length );

  uint burst_length = UINT_MAX;
  for( uint j = 0; j < cyclic_shifts.size(); j++ )
  {
    vector< uint > bit_place_values;
    for( uint i = 0; i < syndrome_length; i++ )
    {
      if( ( ( cyclic_shifts.at( j ) >> i ) & 1 ) == 1 )
      {
        bit_place_values.push_back( i );
      }
    }

    if( !bit_place_values.empty() )
    {
      uint largest_pv = bit_place_values.at(
        bit_place_values.size() - 1 );
      uint smallest_pv = bit_place_values.at( 0 );
      uint this_burst_length = largest_pv - smallest_pv + 1;
      burst_length = this_burst_length < burst_length ?
      this_burst_length : burst_length;
    }
    else
    {
      burst_length = 0;
    }
  }
  return burst_length;
}

uint CyclicCode::nearest_neighbor( uint received_word ) const
{
  //determine the coset for the received word
  vector< uint > received_word_coset;
  for( uint word = 0; word < code_words.size(); word++ )
  {
    uint coset_word = received_word ^ code_words.at( word );
    received_word_coset.push_back( coset_word );
  }
  
  //optional print cosets
  /*
    cout << "received word: " << received_word << endl;
    cout << "corresponding coset: ";
    for( uint i = 0; i < received_word_coset.size(); i++ )
    {
    cout << received_word_coset.at( i ) << " ";
    }
    cout << endl;
    cout << endl;
  */

  //determine the least hamming weight in the coset
  uint error_word = received_word_coset.at( 0 );
  uint least_hw = UINT_MAX;
  const uint ZERO_VECTOR = 0;
  
  for( uint i = 0; i < received_word_coset.size(); i++ )
  {
    uint this_hw = hamming_distance( ZERO_VECTOR,
                                     received_word_coset.at( i ) );
    if( this_hw < least_hw )
    {
      least_hw = this_hw;
      error_word = received_word_coset.at( i );
    }
  }
  //return the received word minus the error word
  uint decoded_word = received_word ^ error_word;
  cout << "used NN decoding" << endl;
  return decoded_word;
}

#endif


