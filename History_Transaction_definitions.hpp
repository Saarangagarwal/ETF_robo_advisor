#ifndef _HISTORY_TRANSACTION_HPP_
#define _HISTORY_TRANSACTION_HPP_

#include <cassert>
#include <iomanip>
#include <iostream>

#include "project3.hpp"
#include "History.hpp"
#include "Transaction.hpp"

////////////////////////////////////////////////////////////////////////////////
// Definitions for Transaction class
////////////////////////////////////////////////////////////////////////////////
//
//

// Constructor
// TASK
//
Transaction::Transaction( std::string ticker_symbol,  unsigned int day_date,
        unsigned int month_date,  unsigned int year_date,
        bool buy_sell_trans,  unsigned int number_shares,
        double trans_amount )
{
	symbol=ticker_symbol;
	day=day_date;
	month=month_date;
	year=year_date;

	if(buy_sell_trans==1)
	{
		trans_type = "Buy";
	}

	else if(buy_sell_trans==0)
	{
		trans_type = "Sell";
	}

	shares=number_shares;
	amount=trans_amount;
	trans_id=assigned_trans_id;
	assigned_trans_id++;
	p_next=nullptr;
	acb_per_share=0;
	acb=0;
	cgl=0;
	share_balance=0;


}



// Destructor
// TASK
//
Transaction::~Transaction(){

}



// TASK
// Overloaded < operator.
//


bool Transaction::operator<(Transaction const &other)
{
	if(this->year < other.year)
	{
		return true;
	}
	else if(this->year==other.year)
	{
		if(this->month < other.month)
		{
			return true;
		}
		else if(this->month == other.month)
		{
			if(this->day < other.day)
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	return false;
}


// GIVEN
// Member functions to get values. 
//
std::string Transaction::get_symbol() const { return symbol; }
unsigned int Transaction::get_day() const { return day; }
unsigned int Transaction::get_month() const { return month; }
unsigned int Transaction::get_year() const { return year; }
unsigned int Transaction::get_shares() const { return shares; }
double Transaction::get_amount() const { return amount; }
double Transaction::get_acb() const { return acb; }
double Transaction::get_acb_per_share() const { return acb_per_share; }
unsigned int Transaction::get_share_balance() const { return share_balance; }
double Transaction::get_cgl() const { return cgl; }
bool Transaction::get_trans_type() const { return (trans_type == "Buy") ? true: false ; }
unsigned int Transaction::get_trans_id() const { return trans_id; }
Transaction *Transaction::get_next() { return p_next; }

// GIVEN
// Member functions to set values. 
//
void Transaction::set_acb( double acb_value ) { acb = acb_value; }
void Transaction::set_acb_per_share( double acb_share_value ) { acb_per_share = acb_share_value; }
void Transaction::set_share_balance( unsigned int bal ) { share_balance = bal ; }
void Transaction::set_cgl( double value ) { cgl = value; }
void Transaction::set_next( Transaction *p_new_next ) { p_next = p_new_next; }

// GIVEN
// Print the transaction.
//
void Transaction::print() {
  std::cout << std::fixed << std::setprecision(2);
  std::cout << std::setw(4) << get_trans_id() << " " 
    << std::setw(4) << get_symbol() << " " 
    << std::setw(4) << get_day() << " "  
    << std::setw(4) << get_month() << " " 
    << std::setw(4) << get_year() << " ";


  if ( get_trans_type() ) { 
    std::cout << "  Buy  "; 
  } else { std::cout << "  Sell "; }

  std::cout << std::setw(4) << get_shares() << " " 
    << std::setw(10) << get_amount() << " " 
    << std::setw(10) << get_acb() << " " << std::setw(4) << get_share_balance() << " "
    << std::setw(10) << std::setprecision(3) << get_acb_per_share() << " "
    << std::setw(10) << std::setprecision(3) << get_cgl() 
    << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
// Definitions for the History class
////////////////////////////////////////////////////////////////////////////////
///
//


// Constructor
// TASK
//
History::History()
{
	p_head=nullptr;
}

// Destructor
// TASK
//
History::~History()
{

	while(p_head  != nullptr)
	{
		Transaction *garbage = p_head;
		p_head = p_head -> get_next();
		delete garbage;
		garbage = nullptr;
	}

	delete p_head;
	p_head = nullptr;

}

// TASK
// read_transaction(...): Read the transaction history from file. 
//
void History::read_history()
{
	ece150::open_file();

	while(ece150::next_trans_entry())
	{
		Transaction *ptr = new Transaction{
								ece150::get_trans_symbol(),
								ece150::get_trans_day(),
								ece150::get_trans_month(),
								ece150::get_trans_year(),
								ece150::get_trans_type(),
								ece150::get_trans_shares(),
								ece150::get_trans_amount()};

		insert(ptr);
	}
}

// insert(...): Insert transaction into linked list.
//
void History::insert( Transaction * p_new_trans)
{

	Transaction * p_traverse {p_head};

		// move p_traverse to the last transaction

	if(p_head==nullptr)
	{
		p_head=p_new_trans;

	}

	if(p_traverse!=nullptr)
	{
		while (p_traverse->get_next()!=nullptr)
		{
			p_traverse = p_traverse->get_next();
		}

		p_traverse-> set_next(p_new_trans);
	}

}



// TASK
// sort_by_date(): Sort the linked list by trade date.


void History::sort_by_date()
{
	Transaction *new_head = nullptr, *temp = nullptr;
	Transaction *p_compare = nullptr;

	while(p_head != nullptr)
	{
		temp = p_head;
		p_head = p_head -> get_next();

		if(new_head == nullptr)
		{
			new_head = temp;
			temp -> set_next(nullptr);

		}

		else
		{
			p_compare = new_head;


			if(*temp < *p_compare)
			{
				temp -> set_next(new_head);
				new_head = temp;
			}

			else
			{
				while((p_compare -> get_next() != nullptr) &&
						(*(p_compare -> get_next()) < *temp)        )
				{

					p_compare = p_compare -> get_next();

				}

				temp -> set_next((p_compare -> get_next()));
				p_compare -> set_next(temp);
			}

	}
	}
	p_head = new_head;

}

// TASK
// update_acb_cgl(): Updates the ACB and CGL values. 
//

void History::update_acb_cgl()
{
	//computing acb, acb_per_share, share_balance and cgl
	Transaction *traverse = p_head;
	double sum=0, acb_p_s;
	unsigned int share=0;
	unsigned int prev_share_balance=0;
	double prev_acb=0;
	double acb_amt=0;



	while(traverse != nullptr)
	{


		if(traverse -> get_trans_type() == 1)  //buy
		{
			sum=acb_amt;
			sum = sum + (traverse -> get_amount());
			traverse -> set_acb(sum);
			acb_amt = sum;

			share = share + (traverse -> get_shares());
			traverse -> set_share_balance(share);

			acb_p_s= (traverse->get_acb())/(traverse->get_share_balance());
			prev_acb = acb_p_s;

			traverse -> set_acb_per_share(acb_p_s);

			prev_share_balance = traverse -> get_share_balance();
		}

		else if(traverse -> get_trans_type() == 0)   //sell
		{

			traverse -> set_acb_per_share(prev_acb);

			double k=(acb_amt - (traverse -> get_acb_per_share() * traverse -> get_shares())) ;//////
			traverse -> set_acb	(k);
			acb_amt = k;

			traverse -> set_share_balance
									( (prev_share_balance) - (traverse -> get_shares())   );

			traverse -> set_cgl(
					traverse -> get_amount()
						- (traverse -> get_shares() * (traverse -> get_acb_per_share())   ));


			prev_share_balance = traverse -> get_share_balance();

			share -= traverse -> get_shares();

		}


		traverse = traverse -> get_next();

	}

}

// TASK
// compute_cgl(): )Compute the ACB, and CGL.
//

double History::compute_cgl( unsigned int year )
{
	Transaction *traveller = p_head;
	double net_cgl=0;

	while(traveller != nullptr)
	{
		if(traveller -> get_year() == year)
		{
			net_cgl += traveller -> get_cgl();
		}

		traveller = traveller->get_next();
	}

}


// TASK
// print() Print the transaction history.
//


void History::print()
{
	Transaction * p_traverse {p_head};
	while(p_traverse != nullptr)
		{
			p_traverse->print();
			p_traverse=p_traverse->get_next();
		}

}


// GIVEN//
// get_p_head(): Full access to the linked list.
// 
Transaction *History::get_p_head() { return p_head; }


#endif

