extern	sec_index();
extern	create();
extern	display();
extern  dest_const();

int	(*Func[])() =
{
	sec_index,
	create,
	display,
	dest_const,
};
