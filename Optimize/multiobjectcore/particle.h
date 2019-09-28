class Particle
{
public:
	//���캯��
	Particle(int, int, const double *, const double *, const double *, double, double, const double *, const double *,void(*)(Particle *));
	//��ֵ���������
	Particle& operator =(const Particle& str);
	//���ƹ��캯��
	Particle(const Particle& str);

	//��������
	~Particle();
	
	//���ӳ�ʼ��
	void inicializeParticle();
	//����λ�ø���
	void updateParticle(const double*);
	//�������ֵ����
	
	//�趨����ֵ
	void setValue(const double *_value);
	//�趨Լ������̶�
	void setConstraits(double _constraits);
	//�趨������
	void setFeasible(bool _feasible);
	//�趨��ռ�Ŵ���
	void setParetoDominance(int _paretoDominance);
	//���ر�ռ�Ŵ���
	int getParetoDominance();
	//��������λ��
	const double *getPosition();
	//���������ٶ�
	//��������ֵ
	const double *getValue();



	//���ر�������
	int getNumberOfVariables();
	//����Ŀ�����
	int getNumberOfObjective();
		
	//�����������λ��
	const double *getBestPosition();
	//������������ٶ�
	//�����������ֵ
	const double *getBestValue();
	
	//���ؿ�����
	bool getFeasible();
	bool getBestFeasible();
	

	//���Print
	void printParticle();
	void printParticleBest();

private:
	//��������
	int numberOfVariables;
	//�Ż�Ŀ�����
	int numberOfObjectives;

	//����λ��
	double *position;
	//�����ٶ�
	double *velocity;
	//����ֵ
	double *value;
	//�������λ��
	double *bestPosition;
	//�������ֵ
	double *bestValue;
	//���ӿ�����
	bool feasible;
	bool bestFeasible;


	//Ȩ��
	static const double *weight;
	static const double *vari;
	static double c1, c2;
	//Լ�������Ż������н����趨��
	double constraits;
	//���ӱ�ռ�Ŵ���
	int paretoDominance;


	//����λ���Ͻ�
	static const double *upperBounds;
	//����λ���½�
	static const double *lowerBounds;
	//��������ٶ�
	static const double *maxVelocity;

	//��������ֵ
	static void(*functionPtr) (Particle *);
	//�����������
	void updateParticleBest();
};


