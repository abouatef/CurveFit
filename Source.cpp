#include "ccc_win.h"
#include <cmath>
#include <iostream>
#include <sstream>

#define Size 200 //division is what's holding it at 99 so reduce the stack usage in division reduce internal function usage
#define period 70 //speed of spinnig
#define PI 3.14159265359


class rect
{
private:
	Point TL, BR;
public:
	void set_TopLeft(Point topleft)
	{
		TL = topleft;
	}
	void set_BottomRight(Point bottomright)
	{
		BR = bottomright;
	}
	void move(float dx, float dy)
	{
		TL.move(dx, dy);
		BR.move(dx, dy);
	}
	void plot(void)
	{
		Line lT(TL, Point(BR.get_x(), TL.get_y())), lD(BR, Point(TL.get_x(), BR.get_y())), lR(BR, Point(BR.get_x(), TL.get_y())), lL(TL, Point(TL.get_x(), BR.get_y()));
		cwin << lT << lD << lR << lL;
	}
};

class complex //we always deal with real and imaginary out side the class for simplicity
{
private:
	float real, img; //we don't need magnitude & phase for every member (memory)
public: //only (void) print will work so to choose bet. precision, range, auto choose your print
	void set_value(float realIn, float imgIn)
	{
		real = realIn;
		img = imgIn;
	}
	void set_real(float realIn)
	{
		real = realIn;
	}
	void set_img(float imgIn)
	{
		img = imgIn;
	}
	void equalize(complex var)
	{
		real = var.real;
		img = var.img;
	}
	float get_real(void)const
	{
		return real;
	}
	float get_imaginary(void)const
	{
		return img;
	}
	float get_phase(char format)const //'R' for rads , 'D' for degrees
	{
		switch (format)
		{
		case 'd':
		case 'D':
			return atan2(img, real) * 180 / PI;
		case'r':
		case'R':
			return atan2(img, real);
		}
	}
	float get_magnitude(void)const
	{
		return sqrt(pow(real, 2) + pow(img, 2));
	}
	bool is_equal(complex var)const
	{
		if (real == var.real && img == var.img)
			return true;
		return false;
	}
	void add(complex first, complex second)
	{
		real = first.real + second.real;
		img = first.img + second.img;
	}
	void subtract(complex first, complex second)
	{
		real = first.real - second.real;
		img = first.img - second.img;
	}
	void multiply(complex first, complex second)
	{
		real = (first.real*second.real) - (first.img*second.img);
		img = (first.real*second.img) + (first.img*second.real);
	}
	void conjugate(complex var)
	{
		real = var.real;
		img = var.img*(-1);
	}
	int divide(complex first, complex second)
	{
		if (second.real == 0 && second.img == 0)
		{
			cout << "Can Not Divide By 0";
			return 1;
		}
		real = (first.c_multiply(second.cconjugate())).real / second.c_multiply(second.cconjugate()).real;
		img = (first.c_multiply(second.cconjugate())).img / second.c_multiply(second.cconjugate()).real;
		return 0;
	}
	void power(complex first, float powr)
	{
		if (powr == 0)
		{
			set_value(1, 0);
			return;
		}
		else if (abs((powr - (int)powr)) < 0.0005) //almost an integer
		{
			equalize(first);
			for (int n = 1; n < abs(round(powr)); n++)
				equalize(first.c_multiply((*this)));
			if (powr > 0)
				return;
			else
			{
				complex one;
				one.set_value(1, 0);
				equalize(one.c_divide((*this)));
			}
		}
		else //rational power
		{
			float denom_pow = (abs((powr - (int)powr)));

			equalize(first);
			for (int n = 1; n < round((int)powr*(1 / denom_pow)) + 1; n++) //power 12.5 = 25/2
				equalize(first.c_multiply((*this))); //done with the intgeral part of the power

			complex A, B;
			A.set_value(pow((*this).get_magnitude(), (denom_pow)), 0);
			B.set_value(cos(denom_pow*(*this).get_phase('R')), sin(denom_pow*(*this).get_phase('R')));
			equalize(A.c_multiply(B)); //now (*this) will have the value of the first^abs(power)

			if (powr > 0)
				return;
			else
			{
				complex one;
				one.set_value(1, 0);
				equalize(one.c_divide((*this)));
			}
		}
	}
	complex c_power(float powr)
	{
		complex temp;
		if (powr == 0)
		{
			temp.set_value(1, 0);
			return temp;
		}
		else if (abs((powr - (int)powr)) < 0.0005) //almost an integer
		{
			temp.equalize((*this));
			for (int n = 1; n < abs(round(powr)); n++)
				temp.equalize((*this).c_multiply(temp));
			if (powr > 0)
				return temp;
			else
			{
				complex one;
				one.set_value(1, 0);
				temp.equalize(one.c_divide(temp));
			}
		}
		else //rational power
		{
			float denom_pow = (abs((powr - (int)powr)));

			temp.equalize((*this));
			for (int n = 1; n < round((int)powr*(1 / denom_pow)) + 1; n++) //power 12.5 = 25/2
				temp.equalize((*this).c_multiply(temp)); //done with the intgeral part of the power

			complex A, B;
			A.set_value(pow(temp.get_magnitude(), (denom_pow)), 0);
			B.set_value(cos(denom_pow*temp.get_phase('R')), sin(denom_pow*temp.get_phase('R')));
			temp.equalize(A.c_multiply(B)); //now (*this) will have the value of the first^abs(power)

			if (powr > 0)
				return temp;
			else
			{
				complex one;
				one.set_value(1, 0);
				temp.equalize(one.c_divide(temp));
			}
		}
		return temp;
	}
	complex p_r(float magnitude, float phase) //polar to rectangular just for internal use
	{
		complex temp;
		temp.set_value(magnitude*cos(phase), magnitude*sin(phase));//use real as magnitude , img as phase
		return temp;
	}
	complex c_add(complex first)
	{
		complex temp;
		temp.real = first.real + real;
		temp.img = first.img + img;
		return temp;
	}
	complex c_subtract(complex first)
	{
		complex temp;
		temp.real = real - first.real;
		temp.img = img - first.img;
		return temp;
	}
	complex c_multiply(complex first)
	{
		complex temp;
		temp.real = (real*first.real) - (first.img*img);
		temp.img = (first.real*img) + (first.img*real);
		return temp;
	}
	complex cconjugate(void)
	{
		complex temp;
		temp.real = real;
		temp.img = img*(-1);
		return temp;
	} //complex conjugate
	complex c_divide(complex second) // doesn't have error detection
	{
		if (second.real == 0 && second.img == 0)
		{
			cout << "Error!\nCan Not Divide By 0";
		}
		complex temp; //denominator can't = 0
		temp.real = ((*this).c_multiply(second.cconjugate())).real / second.c_multiply(second.cconjugate()).real;
		temp.img = ((*this).c_multiply(second.cconjugate())).img / second.c_multiply(second.cconjugate()).real;
		return temp;
	}
private: //not used
	complex r_p() //rectangular to polar just for internal use
	{
		complex temp;
		temp.set_value(sqrt(pow(real, 2) + pow(img, 2)), (atan2(img, real) * 180));//use real as magnitude , img as phase
		return temp;
	}
	complex p_r() //polar to rectangular just for internal use
	{
		complex temp;
		temp.set_value(real*cos(img), real*sin(img));//use real as magnitude , img as phase
		return temp;
	}
};

class vector_m
{
private:
	complex vector[Size];
	short count;
public:
	void set_element(int pos, complex value)
	{
		vector[pos].equalize(value);
	}
	complex get_element(int pos)const
	{
		return vector[pos];
	}
	void set_count(short no)
	{
		count = no;
	}
	short get_count(void)const
	{
		return count;
	}
	void equalize(vector_m var)
	{
		for (short n = 0; n < var.count; n++)
		{
			vector[n].equalize(var.vector[n]);
		}
		count = var.count;
	}
	void multiply(vector_m before, complex factor)
	{
		for (short n = 0; n < count; n++)
		{
			vector[n].equalize(before.vector[n].c_multiply(factor));
		}
		count = before.get_count();
	}
	void add(vector_m first, vector_m second)
	{
		if (first.get_count() != second.get_count())
			return;
		for (short n = 0; n < count; n++)
		{
			vector[n].equalize(first.vector[n].c_add(second.vector[n]));
		}
		count = first.get_count();
	}
	vector_m v_multiply(complex factor)
	{
		vector_m temp;
		for (short n = 0; n < count; n++)
		{
			temp.vector[n].equalize(vector[n].c_multiply(factor));
		}
		temp.set_count(count);
		return temp;
	}
	vector_m v_add(vector_m second)
	{
		/*if (count != second.get_count())
		return;*/
		vector_m temp;
		for (short n = 0; n < count; n++)
		{
			temp.vector[n].equalize(vector[n].c_add(second.vector[n]));
		}
		temp.set_count(count);
		return temp;
	}
	bool is_zero_vec()const
	{
		short zeros = 0;
		for (short n = 0; n < count; n++)
			if (vector[n].get_real() == 0 && vector[n].get_imaginary() == 0)
				zeros++;
		if (zeros == count)
			return true;
		return false;
	}
	bool is_zero_vec_inverseSpecial(short count)const
	{
		short zeros = 0;
		for (short n = 0; n < count; n++)
			if (vector[n].get_real() == 0 && vector[n].get_imaginary() == 0)
				zeros++;
		if (zeros == count)
			return true;
		return false;
	}
};

class matrix //for every error we encounter the function will print the error then return 1 & for success it'll return 0 so that in the main if a function returned 1 we'll repeat the input
{            //in the main if the returned value is 1 we'll ask if the user wants to try again and then goto
private:
	complex mat[Size][Size];
	short rows, columns;
public: //we'll not define these functions returning matrix because it'll elemenate the error checking or we'll have to find an alternative way to check errors
	void set_element(short row, short column, complex value)
	{
		mat[row][column].equalize(value);
	}
	void set_dimentions(short row, short column)
	{
		rows = row;
		columns = column;
	}
	void set_a_row(int position, matrix row) //position starts from 0
	{
		if (row.columns != 1)
			return;
		for (short n = 0; n < columns; n++) //if the row is shorter than the matrix it leaves the rest as it is
		{
			if (n >= row.columns)
				break;
			mat[position][n].equalize(row.mat[0][n]);
		}
	}
	void set_a_column(int position, matrix column) //position starts from 0
	{
		if (column.columns != 1)
			return;
		for (short n = 0; n < rows; n++)   //if the column is shorter than the matrix it leaves the rest as it is
		{
			if (n >= column.rows)
				break;
			mat[n][position].equalize(column.mat[n][0]);
		}
	}
	void set_a_row(int position, vector_m row) //position starts from 0
	{
		for (short n = 0; n < columns; n++) //if the row is shorter than the matrix it leaves the rest as it is
		{
			if (n >= row.get_count())
				break;
			mat[position][n].equalize(row.get_element(n));
		}
	}
	void set_a_column(int position, vector_m column) //position starts from 0
	{
		for (short n = 0; n < rows; n++)   //if the column is shorter than the matrix it leaves the rest as it is
		{
			if (n >= column.get_count())
				break;
			mat[n][position].equalize(column.get_element(n));
		}
	}
	void get_a_row(int position, matrix full) //position starts from 0
	{
		for (short n = 0; n < full.columns; n++)
		{
			mat[0][n].equalize(full.mat[position][n]);
		}
		columns = full.columns;
		rows = 1;
	}
	void get_a_column(int position, matrix full) //position starts from 0
	{
		for (short n = 0; n < full.rows; n++)
		{
			mat[n][0].equalize(full.mat[n][position]);
		}
		columns = 1;
		rows = full.rows;
	}
	vector_m get_a_row(int position) //position starts from 0
	{
		vector_m row;
		for (short n = 0; n < columns; n++)
		{
			row.set_element(n, mat[position][n]);
		}
		row.set_count(columns);
		return row;
	}
	vector_m get_a_column(int position) //position starts from 0
	{
		vector_m column;
		for (short n = 0; n < rows; n++)
		{
			column.set_element(n, mat[n][position]);
		}
		column.set_count(rows);
		return column;
	}
	void swap_rows(int from, int to)
	{
		vector_m first, second;
		first.equalize((*this).get_a_row(from));
		second.equalize((*this).get_a_row(to));
		(*this).set_a_row(to, first);
		(*this).set_a_row(from, second);
	}
	void swap_columns(int from, int to)
	{
		vector_m first, second;
		first.equalize((*this).get_a_column(from));
		second.equalize((*this).get_a_column(to));
		(*this).set_a_column(to, first);
		(*this).set_a_column(from, second);
	}
	void set_rows(short row)
	{
		rows = row;
	}
	void set_columns(short column)
	{
		columns = column;
	}
	short get_rows(void)
	{
		return rows;
	}
	short get_columns(void)
	{
		return columns;
	}
	complex get_element(short row, short column)const
	{
		return mat[row][column];
	}
	int add(matrix first, matrix second)
	{
		if ((first.rows != second.rows) || (first.columns != second.columns))
		{
			cout << "Error!\nImproper Dimentions";// the two matrixes don't have the same dimentions
			return 1;
		}
		else
		{
			complex temp;
			for (int row = 0; row < first.rows; row++)
			{
				for (int column = 0; column < first.columns; column++)
				{
					mat[row][column].equalize(first.mat[row][column].c_add(second.mat[row][column]));
				}
			}
		}
		rows = first.rows;
		columns = first.columns;
		return 0;
	}
	int subtract(matrix first, matrix second)
	{
		if ((first.rows != second.rows) || (first.columns != second.columns))
		{
			cout << "Error!\nImproper Dimentions";// the two matrixes don't have the same dimentions
			return 1;
		}
		else
		{
			complex temp;
			for (int row = 0; row < first.rows; row++)
			{
				for (int column = 0; column < first.columns; column++)
				{
					mat[row][column].equalize(first.mat[row][column].c_subtract(second.mat[row][column]));
				}
			}
		}
		rows = first.rows;
		columns = first.columns;
		return 0;
	}
	void intialize(void)
	{
		for (short row = 0; row < rows; row++) //first input rows
		{
			for (short column = 0; column < columns; column++) //second input columns
			{
				mat[row][column].set_value(0, 0);
			}
		}
	}
	void equalize(matrix var)
	{
		for (short m = 0; m < var.rows; m++)
		{
			for (short n = 0; n < var.columns; n++)
			{
				mat[m][n].equalize(var.mat[m][n]);
			}
		}
		rows = var.rows;
		columns = var.columns;
	}
	int multiply(matrix first, matrix second)
	{
		if (first.columns != second.rows) //m*n,k*j ---> n=k , out [m,j]
		{
			cout << "Error!\nImproper Dimentions";
			return 1;// the two matrixes don't have proper dimentions
		}
		else
		{   // intialization with zeros
			for (short row = 0; row < first.rows; row++) //first input rows
			{
				for (short column = 0; column < second.columns; column++) //second input columns
				{
					mat[row][column].set_value(0, 0);
				}
			}
			complex temp;
			for (short row = 0; row < first.rows; row++) //first input rows
				for (short column = 0; column < second.columns; column++) //second input columns
					for (short x = 0; x < first.columns; x++)
						mat[row][column].add(mat[row][column], first.mat[row][x].c_multiply(second.mat[x][column]));
		}
		rows = first.rows;
		columns = second.columns;
		return 0;
	}
	int transpose(matrix var)
	{
		for (short m = 0; m < var.rows; m++)
		{
			for (short n = 0; n < var.columns; n++)
			{
				mat[n][m].equalize(var.mat[m][n]);
			}
		}
		rows = var.columns;
		columns = var.rows;
		return 0;
	}
	int power(matrix var, string pow1)
	{
		for (int i = 0; i < pow1.size(); i++)
		{
			switch (pow1[i])
			{
			case'0':
			case'1':
			case'2':
			case'3':
			case'4':
			case'5':
			case'6':
			case'7':
			case'8':
			case'9':
				break;
			default:
				cout << "Error!\nThe Power Is Not a Positive Intger"; //power is not an integer Or is not +ve
				return 1;
			}
		}
		float pow = atof(pow1.c_str());
		if (var.columns != var.rows) //m*n,k*j ---> n=k , out [m,j]
		{
			cout << "Error!\nImproper Dimentions";
			return 1;// not a square matrix
		}
		else
		{
			if (pow == 0) //identity Matrix
			{
				for (short m = 0; m < var.rows; m++)
				{
					for (short n = 0; n < var.columns; n++)
					{
						mat[m][n].set_value(0, 0);
					}
					mat[m][m].set_value(1, 0);
				}
				rows = var.rows;
				columns = var.columns;
			}
			else if (pow == 1)
				for (short m = 0; m < var.rows; m++)
					for (short n = 0; n < var.columns; n++)
						(*this).equalize(var);
			else
			{
				//matrix TempCopy;
				(*this).equalize(var);
				for (int Power = 0; Power < (pow - 1); Power++) //Power Loop
				{
					(*this).multiply((*this), var);
				}
			}
		}
		rows = var.rows;
		columns = var.columns;
		return 0;
	}
	complex determinant(bool &state) //still there's a bug when trying to s swap 
	{
		state = 0;
		if (columns != rows)
		{
			cout << "Error!\nNot a Square matrix!"; // Not a square matrix
			state = 1;
			return mat[0][0]; //null value won't be used
		}
		else
		{
			vector_m temp1;
			complex one, minus_one, diag_element;
			one.set_value(1, 0);
			minus_one.set_value(-1, 0);
			for (short diag = 0; diag < rows - 1; diag++) //chooses the element of the diagonal that we're eliminating under
			{
				for (short u_diag = diag + 1; u_diag < rows; u_diag++) //under each diagonal element we'll eliminate
				{
					if (mat[u_diag][diag].get_real() == 0 && mat[u_diag][diag].get_imaginary() == 0)
						continue;
					temp1.equalize((*this).get_a_row(u_diag));
					if (mat[diag][diag].get_real() == 1 && mat[diag][diag].get_imaginary() == 0)
						(*this).set_a_row(u_diag, temp1.v_add((*this).get_a_row(diag).v_multiply(mat[u_diag][diag].c_multiply(minus_one))));
					else if (mat[diag][diag].get_real() == -1 && mat[diag][diag].get_imaginary() == 0)
						(*this).set_a_row(u_diag, temp1.v_add((*this).get_a_row(diag).v_multiply(mat[u_diag][diag])));
					else if (mat[diag][diag].get_real() == 0 && mat[diag][diag].get_imaginary() == 0)
					{
						for (short trans = u_diag; trans < rows; trans++)
						{
							if (mat[trans][diag].get_real() != 0 || mat[trans][diag].get_real() != 0)
							{
								(*this).swap_rows(diag, trans);
								(*this).set_a_row(trans, (*this).get_a_row(trans).v_multiply(minus_one));
								u_diag--;
								break;
							}
							else
							{
								if (trans == rows - 1)
								{
									diag_element.set_value(0, 0);
									return diag_element;
								}
							}
						}
					}
					else
						(*this).set_a_row(u_diag, temp1.v_add((*this).get_a_row(diag).v_multiply((one.c_divide(mat[diag][diag])).c_multiply((mat[u_diag][diag].c_multiply(minus_one))))));
				}
			}
			return (*this).multiply_diagonal_elements();
		}
	}
	vector_m cramer(vector_m ans)
	{
		complex delta, delta_i;
		delta.equalize(determinant());
		vector_m unknowns, temp_column;
		unknowns.set_count(rows);
		for (int i = 0; i < rows; i++)
		{
			temp_column.equalize(get_a_column(i));
			set_a_column(i, ans);
			delta_i.equalize(determinant());
			unknowns.set_element(i, delta_i.c_divide(delta));
			set_a_column(i, temp_column);
		}
		return unknowns;
	}
	int eliminate(matrix var)
	{ //change elimination to be the method of inversing [1 0 0;0 1 0;0 0 1] 
		vector_m temp1;
		complex one, minus_one, diag_element;
		one.set_value(1, 0);
		minus_one.set_value(-1, 0);
		for (short diag = 0; (diag < var.rows - 1) && (diag < var.columns); diag++) //chooses the element of the diagonal that we're eliminating under
		{
			for (short u_diag = diag + 1; u_diag < var.rows; u_diag++) //under each diagonal element we'll eliminate
			{
				if (var.mat[u_diag][diag].get_real() == 0 && var.mat[u_diag][diag].get_imaginary() == 0)
					continue;
				temp1.equalize(var.get_a_row(u_diag));
				if (var.mat[diag][diag].get_real() == 1 && var.mat[diag][diag].get_imaginary() == 0)
					var.set_a_row(u_diag, temp1.v_add(var.get_a_row(diag).v_multiply(var.mat[u_diag][diag].c_multiply(minus_one))));
				else if (var.mat[diag][diag].get_real() == -1 && var.mat[diag][diag].get_imaginary() == 0)
					var.set_a_row(u_diag, temp1.v_add(var.get_a_row(diag).v_multiply(var.mat[u_diag][diag])));
				else if (var.mat[diag][diag].get_real() == 0 && var.mat[diag][diag].get_imaginary() == 0)
				{
					for (short trans = u_diag; trans < var.rows; trans++)
					{
						if (var.mat[trans][diag].get_real() != 0 || var.mat[trans][diag].get_real() != 0)
						{
							var.swap_rows(diag, trans);
							var.set_a_row(trans, var.get_a_row(trans).v_multiply(minus_one));
							u_diag--;
							break;
						}
					}
				}
				else
					var.set_a_row(u_diag, temp1.v_add(var.get_a_row(diag).v_multiply((one.c_divide(var.mat[diag][diag])).c_multiply((var.mat[u_diag][diag].c_multiply(minus_one))))));
			}
		}
		(*this).equalize(var);
		return 0;
	}
	int rank(void)
	{
		vector_m temp1;
		complex one, minus_one, diag_element;
		short zeroVectors;
		one.set_value(1, 0);
		minus_one.set_value(-1, 0);
		for (short diag = 0; (diag < rows - 1) && (diag < columns); diag++) //chooses the element of the diagonal that we're eliminating under
		{
			for (short u_diag = diag + 1; u_diag < rows; u_diag++) //under each diagonal element we'll eliminate
			{
				if (mat[u_diag][diag].get_real() == 0 && mat[u_diag][diag].get_imaginary() == 0)
					continue;
				temp1.equalize(get_a_row(u_diag));
				if (mat[diag][diag].get_real() == 1 && mat[diag][diag].get_imaginary() == 0)
					set_a_row(u_diag, temp1.v_add(get_a_row(diag).v_multiply(mat[u_diag][diag].c_multiply(minus_one))));
				else if (mat[diag][diag].get_real() == -1 && mat[diag][diag].get_imaginary() == 0)
					set_a_row(u_diag, temp1.v_add(get_a_row(diag).v_multiply(mat[u_diag][diag])));
				else if (mat[diag][diag].get_real() == 0 && mat[diag][diag].get_imaginary() == 0)
				{
					for (short trans = u_diag; trans < rows; trans++)
					{
						if (mat[trans][diag].get_real() != 0 || mat[trans][diag].get_real() != 0) //swap with the first row that does not have zero in the same place
						{
							swap_rows(diag, trans);
							set_a_row(trans, get_a_row(trans).v_multiply(minus_one));
							u_diag--;
							break;
						}
					}
				}
				else
					set_a_row(u_diag, temp1.v_add(get_a_row(diag).v_multiply((one.c_divide(mat[diag][diag])).c_multiply((mat[u_diag][diag].c_multiply(minus_one))))));
			}
		}
		return rows - (*this).no_of_zero_vectors();
	}
	int inverse(matrix var) //better in the bigger matrixes but alittle bit worse (not always) in the small ones
	{
		if (var.rows != var.columns)
		{
			cout << "Error!\nNot a Square matrix";// Not a square matrix
			return 1;
		}
		else
		{
			matrix bigMat;


			//first putting the matrix next to I
			for (short m = 0; m < var.rows; m++)
			{
				for (short n = 0; n < var.columns * 2; n++)
				{
					if (n >= var.columns)
					{
						bigMat.mat[m][n].set_value(0, 0);
					}
					else
						bigMat.mat[m][n].equalize(var.mat[m][n]);
				}
				bigMat.mat[m][m + var.columns].set_value(1, 0);
			}
			bigMat.rows = var.rows;
			bigMat.columns = var.columns * 2;


			//getting zeros under the diagonal
			vector_m temp1;
			complex one, minus_one, diag_element;
			one.set_value(1, 0);
			minus_one.set_value(-1, 0);
			for (short diag = 0; (diag < var.rows - 1) && (diag < var.columns); diag++) //chooses the element of the diagonal that we're eliminating under
			{
				for (short u_diag = diag + 1; u_diag < var.rows; u_diag++) //under each diagonal element we'll eliminate
				{
					if (bigMat.mat[u_diag][diag].get_real() == 0 && bigMat.mat[u_diag][diag].get_imaginary() == 0)
						continue;
					temp1.equalize(bigMat.get_a_row(u_diag));
					if (bigMat.mat[diag][diag].get_real() == 1 && bigMat.mat[diag][diag].get_imaginary() == 0)
						bigMat.set_a_row(u_diag, temp1.v_add(bigMat.get_a_row(diag).v_multiply(bigMat.mat[u_diag][diag].c_multiply(minus_one))));
					else if (bigMat.mat[diag][diag].get_real() == -1 && bigMat.mat[diag][diag].get_imaginary() == 0)
						bigMat.set_a_row(u_diag, temp1.v_add(bigMat.get_a_row(diag).v_multiply(bigMat.mat[u_diag][diag])));
					else if (bigMat.mat[diag][diag].get_real() == 0 && bigMat.mat[diag][diag].get_imaginary() == 0)
					{
						for (short trans = u_diag; trans < bigMat.rows; trans++)
						{
							if (bigMat.mat[trans][diag].get_real() != 0 || bigMat.mat[trans][diag].get_real() != 0)
							{
								bigMat.swap_rows(diag, trans);
								bigMat.set_a_row(trans, bigMat.get_a_row(trans).v_multiply(minus_one));
								u_diag--;
								break;
							}
						}
					}
					else
						bigMat.set_a_row(u_diag, temp1.v_add(bigMat.get_a_row(diag).v_multiply((one.c_divide(bigMat.mat[diag][diag])).c_multiply((bigMat.mat[u_diag][diag].c_multiply(minus_one))))));
				}
			}


			//check for zero rows
			for (short m = 0; m < var.rows; m++)
			{
				if (bigMat.get_a_row(m).is_zero_vec_inverseSpecial(var.columns))
				{
					cout << "Error!\nMatrix is not invertible";
					return 1;
				}
			}


			//making the diagonal of our matrix = ones
			for (short m = 0; m < bigMat.rows; m++)
				bigMat.set_a_row(m, bigMat.get_a_row(m).v_multiply(one.c_divide(bigMat.mat[m][m])));


			//eliminating above the diagoanl
			for (short diag = var.rows - 1; diag >= 0; diag--) //chooses the element of the diagonal that we're eliminating under
			{
				for (short u_diag = diag - 1; u_diag >= 0; u_diag--) //under each diagonal element we'll eliminate
				{
					if (bigMat.mat[u_diag][diag].get_real() == 0 && bigMat.mat[u_diag][diag].get_imaginary() == 0)
						continue;
					temp1.equalize(bigMat.get_a_row(u_diag));
					bigMat.set_a_row(u_diag, temp1.v_add(bigMat.get_a_row(diag).v_multiply(bigMat.mat[u_diag][diag].c_multiply(minus_one))));
				}
			}


			//returing the inverse 
			for (short m = 0; m < var.rows; m++)
			{
				for (short n = 0; n < var.columns; n++)
				{
					mat[m][n].equalize(bigMat.mat[m][n + var.columns]);
				}
			}
			rows = var.rows;
			columns = var.columns;
		}
	}
	int divide(matrix first, matrix second)
	{
		//we'll give the imaginaries to this function just to use in the multiply function as matrixes full of zeros instead of defining and intializing 3 new matrixes
		if (first.columns != second.rows || second.columns != second.rows) //m*n,k*j ---> n=k , out [m,j]
		{
			cout << "Error!\nImproper Dimentions";
			return 1;// the two matrixes don't have proper dimentions
		}
		else
		{
			matrix tempInverse;
			if (tempInverse.inverse(second) == 1)
				return 1;
			(*this).multiply(first, tempInverse);
			rows = first.rows;
			columns = second.columns;
			return 0;
		}
	}	
private:
	complex determinant(void) //special error case fo internal use only
	{
		matrix tmat;

		for (short m = 0; m < rows; m++) //instead of equalize (to save some variables from being on the stack)
		{
			for (short n = 0; n < columns; n++)
			{
				tmat.mat[m][n].equalize(mat[m][n]);
			}
		}
		tmat.rows = rows;
		tmat.columns = columns;

		vector_m temp1;
		complex one, minus_one, diag_element;
		one.set_value(1, 0);
		minus_one.set_value(-1, 0);
		for (short diag = 0; diag < tmat.rows - 1; diag++) //chooses the element of the diagonal that we're eliminating under
		{
			for (short u_diag = diag + 1; u_diag < tmat.rows; u_diag++) //under each diagonal element we'll eliminate
			{
				if (tmat.mat[u_diag][diag].get_real() == 0 && tmat.mat[u_diag][diag].get_imaginary() == 0)
					continue;
				temp1.equalize(tmat.get_a_row(u_diag));
				if (tmat.mat[diag][diag].get_real() == 1 && tmat.mat[diag][diag].get_imaginary() == 0)
					tmat.set_a_row(u_diag, temp1.v_add(tmat.get_a_row(diag).v_multiply(tmat.mat[u_diag][diag].c_multiply(minus_one))));
				else if (tmat.mat[diag][diag].get_real() == -1 && tmat.mat[diag][diag].get_imaginary() == 0)
					tmat.set_a_row(u_diag, temp1.v_add(tmat.get_a_row(diag).v_multiply(tmat.mat[u_diag][diag])));
				else if (tmat.mat[diag][diag].get_real() == 0 && tmat.mat[diag][diag].get_imaginary() == 0)
				{
					for (short trans = u_diag; trans < tmat.rows; trans++)
					{
						if (tmat.mat[trans][diag].get_real() != 0 || tmat.mat[trans][diag].get_real() != 0)
						{
							tmat.swap_rows(diag, trans);
							tmat.set_a_row(trans, tmat.get_a_row(trans).v_multiply(minus_one));
							u_diag--;
							break;
						}
						else
						{
							if (trans == tmat.rows - 1)
							{
								diag_element.set_value(0, 0);
								return diag_element;
							}
						}
					}
				}
				else
					tmat.set_a_row(u_diag, temp1.v_add(tmat.get_a_row(diag).v_multiply((one.c_divide(tmat.mat[diag][diag])).c_multiply((tmat.mat[u_diag][diag].c_multiply(minus_one))))));
			}
		}
		return tmat.multiply_diagonal_elements();
	} //to be used inside cramer only
	complex determinant_R(bool &state) //(R) for (Recursion) : causes stack overflow (not Used)
	{
		state = 0;
		if (columns != rows)
		{
			cout << "Error!\nNot a Square matrix!"; // Not a square matrix
			state = 1;
			complex null;
			null.set_value(0, 0);
			return null;
		}
		else if (rows == 1)
		{
			return mat[0][0];
		}
		else if (rows == 2)
		{
			complex det;
			det.equalize((mat[0][0].c_multiply(mat[1][1])).c_subtract(mat[0][1].c_multiply(mat[1][0])));
			return det;
		}
		else
		{
			complex det; matrix Temp;
			det.set_value(0, 0);
			int rowAfterTossing, columnAfterTossing, i;

			for (int c = 0; c < rows; c++)
			{
				for (rowAfterTossing = 0; rowAfterTossing < rows - 1; rowAfterTossing++)
				{
					for (columnAfterTossing = 0, i = 0; columnAfterTossing < rows; columnAfterTossing++)
					{
						if (c == columnAfterTossing)
							continue;
						Temp.mat[rowAfterTossing][i].equalize(mat[rowAfterTossing + 1][columnAfterTossing]);
						i++;
					}
				}
				Temp.rows = rows - 1;
				Temp.columns = columns - 1;
				complex sign;
				sign.set_value(pow(-1, c + 2), 0);
				det.add(det, mat[0][c].c_multiply((Temp.determinant(state)).c_multiply(sign)));
			}
			return det;
		}
	}
	complex matrix_of_coFactors(int d, int c)
	{
		complex det; //to put the determinant that will appear after eliminaiting the row and the column of the element that i'm consiering
		det.set_value(0, 0);
		if (rows == 1)
		{
			return mat[0][0];
		}
		else if (rows == 2)
		{
			det.equalize((mat[0][0].c_multiply(mat[1][1])).c_subtract(mat[0][1].c_multiply(mat[1][0])));
			return det;
		}
		else
		{
			matrix Temp;
			complex sign;
			int rowAfterTossing, columnAfterTossing, i, j;

			for (rowAfterTossing = 0, j = 0; rowAfterTossing < rows; rowAfterTossing++)
			{
				if (d == rowAfterTossing)
					continue;
				for (columnAfterTossing = 0, i = 0; columnAfterTossing < columns; columnAfterTossing++)
				{
					if (c == columnAfterTossing)
						continue;
					Temp.mat[j][i].equalize(mat[rowAfterTossing][columnAfterTossing]);
					i++;
				}
				j++;
			}
			Temp.rows = rows - 1;
			Temp.columns = columns - 1;
			sign.set_value(pow(-1, c + d + 2), 0);
			det.add(det, (Temp.determinant()).c_multiply(sign));
			return det;
		}
	}
	complex multiply_diagonal_elements(void)
	{
		complex temp;
		temp.set_value(1, 0);
		for (int x = 0; x < rows; x++)
		{
			temp.equalize(temp.c_multiply(mat[x][x]));
		}
		return temp;
	}
	short no_of_zero_vectors(void)
	{
		short zero_vecs = 0;
		for (short m = 0; m < rows; m++)
		{
			if (get_a_row(m).is_zero_vec())
				zero_vecs++;
		}
		return zero_vecs;
	}
};

int ccc_win_main()
{
	vector_m coefs, ys;
	matrix matA;
	double min, max;
	min = -10; //default
	max = 10;
	while (1)
	{
	again:
		rect r;
		Point nodes[Size], p1, p2;
		double coofs[Size], temp_num = 1, temp_denom = 1, tempY = 0, n; //coordinates must be square 
		int count = 0;
		short lines;
		string zooms, eqn = "Eqn. is: Y = ";

		/*------------------------------------------intial screen , getting the points------------------------------------------*/

		r.set_TopLeft(Point((min - (min / 50)), (max - (3 * max / 20)))); //to keep the proprtions regardless of the coordinates
		r.set_BottomRight(Point((min - (min / 5.8824)), (max - (3 * max / 10))));
		r.plot();
		cwin << Message(Point((min - (min / 40)), (max - (max / 5.1282))), "STOP");


		cwin << Line(Point(0, max), Point(0, min));
		cwin << Line(Point(min, 0), Point(max, 0));
		for (double l = min; l <= max; l += max / 5)
		{
			cwin << Point(0, l) << Message(Point(0, l), l);
			cwin << Point(l, 0) << Message(Point(l, 0), l);
		}


		while (1)
		{
			nodes[count] = cwin.get_mouse("Specify the points and (stop) to estimate the polynomial");
			if (((nodes[count].get_x() >= (min - (min / 50))) && (nodes[count].get_x() <= (min - (min / 5.8824)))) && ((nodes[count].get_y() >= (max - (3 * max / 10))) && (nodes[count].get_y() <= (max - (3 * max / 20)))))
				break;
			cwin << nodes[count];
			count++;
		}

		/*-------------------------------------------------checking for infinte slope--------------------------------------------------*/

		for (int m = 0; m < count; m++) //this double for loop is to eliminate the bug of slope = infinity
		{
			for (int k = 0; k < count; k++)
			{
				if (k == m)
					continue;
				else if (nodes[m].get_x() == nodes[k].get_x())
				{
					cwin << Message(Point((min - (min / 1.905)), -1 * (min - (min / 1.905)) / 4.75), "There are two Points with the same x");
					cwin.get_mouse("Click to try again");
					cwin.clear();
					goto again;
				}
			}

		}
	zoom:
		n = min;

		/*------------------------------------------calculating the firs piont only (for plotting)----------------------------------------*/

	again0:
		for (int m = 0; m < count; m++)
		{
			for (int j = 0; j < count; j++)
			{
				if (j == m)
					continue;
				temp_num *= n - nodes[j].get_x();
			}
			for (int j = 0; j < count; j++)
			{
				if (j == m)
					continue;
				temp_denom *= nodes[m].get_x() - nodes[j].get_x();
			}
			tempY += nodes[m].get_y()*(temp_num / temp_denom);
			temp_num = 1;
			temp_denom = 1;
		}
		if ((tempY >= max || tempY <= min) && (n <= max&&n >= min))
		{
			tempY = 0;
			n += max / 1000;
			goto again0;
		}
		p2 = Point(n, tempY);
		tempY = 0;

		/*-----------------------------------------------plotting the polynomial-------------------------------------------------*/

		for (n = min + (max / 1000); n <= max; n += (max / 1000))
		{
			for (int m = 0; m < count; m++)
			{
				for (int j = 0; j < count; j++)
				{
					if (j == m)
						continue;
					temp_num *= n - nodes[j].get_x();
				}
				for (int j = 0; j < count; j++)
				{
					if (j == m)
						continue;
					temp_denom *= nodes[m].get_x() - nodes[j].get_x();
				}
				tempY += nodes[m].get_y()*(temp_num / temp_denom);
				temp_num = 1;
				temp_denom = 1;
			}
			if ((tempY >= max || tempY <= min) && (n <= max&&n >= min))
			{
				tempY = 0;
				continue;
			}
			p1 = Point(n, tempY);
			cwin << Line(p1, p2);
			p2 = p1;
			tempY = 0;
		}

		/*--------------------------------------------getting the polynomial-------------------------------------------*/

		ys.set_count(count);
		for (int h = 0; h < count; h++) //using cramer and the given points to calculate the coefficients
		{
			complex x;
			x.set_value(nodes[h].get_y(), 0);
			ys.set_element(h, x);
		}
		matA.set_dimentions(count, count);
		for (int h = 0; h <= count; h++)
		{
			for (int g = 0; g <= count; g++)
			{
				complex x;
				x.set_value(pow(nodes[h].get_x(), count - 1 - g), 0);
				matA.set_element(h, g, x);
			}
		}
		coefs.equalize(matA.cramer(ys));

		//error check:

		if (isnan(coefs.get_element(0).get_real())) //if enq of deg 9 but the points have almost the same y so the X^9 has a very very small coefficient that'd be displayed as -1.#IND00 that is (NaN)(Not a Number) and many other cases
		{
			eqn = "Sorry the eqn. can't be displayed due to precision errors";
			cwin << Message(Point(min - (7 * min / 400), min - (7 * min / 100)), eqn);
			goto error;
		}

		//creating the string out of the coefficients to be displayed:

		for (int h = 0; h < count - 2; h++)//last two terms done manually
		{
			if (coefs.get_element(h).get_real() > 0 && h != 0)
				eqn += '+';
			else if (coefs.get_element(h).get_real() == 0)
				continue;
			eqn += to_string(coefs.get_element(h).get_real()) + "X^" + to_string(count - 1 - h); //convert from coofs to string
		}
		//last two terms
		if (coefs.get_element(count - 2).get_real() > 0 && coefs.get_count() > 2)
			eqn += '+';
		else if (coefs.get_element(count - 2).get_real() == 0 || coefs.get_count() < 2)
			goto zero;
		eqn += to_string(coefs.get_element(count - 2).get_real()) + "X";
	zero:
		if (coefs.get_element(count - 1).get_real() > 0 && coefs.get_count() > 1)
			eqn += '+';
		else if (coefs.get_element(count - 1).get_real() == 0 || coefs.get_count() < 1)
			goto error;
		eqn += to_string(coefs.get_element(count - 1).get_real());

		//splitting the eqn. to be displayed properly: (70 digits is the maxinmum no. of digits that can be displayed in the default screen size)

		for (int j = 0; j < 40; j++)//40 is the max no. of lines (e.g:at 200 point)
		{
			if (eqn.size() > j * 69)
				continue;
			lines = j;
			break;
		}
		for (int j = 0; j < lines; j++)//40 is the max no. of lines (e.g:at 200 point)
		{
			cwin << Message(Point(min - (6 * min / 400), (min - ((lines - j)*(6 * min / 100)))), eqn.substr(69 * j, 69));
		}

		/*---------------------------------------------------zooming phase---------------------------------------------------------*/

	error:
		zooms = cwin.get_string("Enter:retry, '+':zoom in, '-':zoom out, 's':show coordinates: ");
		if (!(((zooms[0] == '-' || zooms[0] == '+' || zooms[0] == 's') && zooms.size() == 1) || zooms.size() == 0) || zooms.size() > 1)
			goto error;
		switch (zooms[0])
		{
		case'+':
			min /= 2;
			max /= 2;
			cwin.coord(min, -min, max, -max);
			cwin.clear();
			r.set_TopLeft(Point((min - (min / 50)), (max - (3 * max / 20)))); //to keep the proprtions regardless of the coordinates
			r.set_BottomRight(Point((min - (min / 5.8824)), (max - (3 * max / 10))));
			r.plot();
			cwin << Message(Point((min - (min / 40)), (max - (max / 5.1282))), "STOP");
			cwin << Line(Point(0, max), Point(0, min));
			cwin << Line(Point(min, 0), Point(max, 0));
			for (double l = min; l <= max; l += max / 5)
			{
				cwin << Point(0, l) << Message(Point(0, l), l);
				cwin << Point(l, 0) << Message(Point(l, 0), l);
			}
			for (int l = 0; l < count; l++)
				cwin << nodes[l];
			eqn = "Eqn. is: Y = ";
			goto zoom;
			break;
		case'-':
			min *= 2;
			max *= 2;
			cwin.coord(min, -min, max, -max);
			cwin.clear();
			r.set_TopLeft(Point((min - (min / 50)), (max - (3 * max / 20)))); //to keep the proprtions regardless of the coordinates
			r.set_BottomRight(Point((min - (min / 5.8824)), (max - (3 * max / 10))));
			r.plot();
			cwin << Message(Point((min - (min / 40)), (max - (max / 5.1282))), "STOP");
			cwin << Line(Point(0, max), Point(0, min));
			cwin << Line(Point(min, 0), Point(max, 0));
			for (double l = min; l <= max; l += max / 5)
			{
				cwin << Point(0, l) << Message(Point(0, l), l);
				cwin << Point(l, 0) << Message(Point(l, 0), l);
			}
			for (int l = 0; l < count; l++)
				cwin << nodes[l];
			eqn = "Eqn. is: Y = ";
			goto zoom;
			break;
		case's':
			cwin.get_mouse("Click to stop coordinates");
			goto error;
			break;
		//we priviously checked for wrong input so it must be emtpy string by now
		}
		cwin.clear();
	}
	return 0;
}

/*int ccc_win_main() //best fitting line
{
	while (1) 
	{
	rect r;
	r.set_TopLeft(Point(-9.8, 8.5));
	r.set_BottomRight(Point(-8.3, 7));
	r.plot();
	cwin << Message(Point(-9.75, 8.05), "STOP");
	double sum_x = 0, sum_y = 0, sum_xy = 0, sum_xx = 0, m;
	int count = 0;
	while (1)
	{
	Point temp;
	temp = cwin.get_mouse("Click to specify the points and (stop) to estimate the line");
	if ((temp.get_x() >= -9.8 && temp.get_x() <= -8.3) && (temp.get_y() >= 7 && temp.get_y() <= 8.5))
	break;
	cwin << temp;
	sum_x += temp.get_x();
	sum_y += temp.get_y();
	sum_xx += temp.get_x()*temp.get_x();
	sum_xy += temp.get_x()*temp.get_y();
	count++;
	}
	m = (sum_xy - sum_x*(sum_y / count)) / (sum_xx - sum_x*(sum_x / count));
	cwin << Line(Point(-10, (sum_y / count) + m*(-10 - (sum_x / count))), Point(10, (sum_y / count) + m*(10 - (sum_x / count))));
	cwin.get_mouse("Click to continue");
	cwin.clear();
	}
	return 0;
}*/



/*int ccc_win_main() //polynomial plotting
{
	Point p1, p2;
	double coofs[1000], temp = 0, x0, xf; //coordinates must be square 
	int deg, count = 0;

	deg = cwin.get_int("please enter the degree of the polynomial: "); //polynomial plotting
	for (int n = 0; n <= deg; n++)
		coofs[n] = cwin.get_double("please enter the coefficients: ");
	x0 = cwin.get_double("please enter the intial x: ");
	xf = cwin.get_double("please enter the final x: ");


	for (int m = 0; m <= deg; m++)
	{
		temp += coofs[m] * pow(x0, deg - m);
	}
	p2 = Point(x0, temp);
	temp = 0;


	for (double n = x0 + 0.01; n <= xf; n += 0.01)
	{
		for (int m = 0; m <= deg; m++)
		{
			temp += coofs[m] * pow(n, deg - m);
		}
		p1 = Point(n, temp);
		cwin << Line(p1, p2);
		p2 = p1;
		temp = 0;
	}
	return 0;
}*/
