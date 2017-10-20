## QFreamWok �ļ���һ��ͨ�õ�Qt���򿪷���ܣ�����C++��д��ʹ��ʱֱ����ӽ���Ŀ�ļ��С�
* ### (1).QMeter��Ϊ���������Ǳ���
    ![github](README_Image/1.png)
    * ʹ�÷�ʽ��
    ```c++
    QMeter *m_pMeter = new QMeter(this);
    m_pMeter->setGeometry(0,0,434,282);
    m_pMeter->setForeground(Qt::white);
    m_pMeter->setFont(QFont("Ubuntu",5));

    QSlider *m_pSlider1 = new QSlider(this);
    m_pSlider1->setGeometry(500,300,400,20);
    m_pSlider1->setOrientation(Qt::Horizontal);

    connect(m_pSlider1,SIGNAL(sliderMoved(int)),m_pMeter1,SLOT(setValue(int)));
    ```
* ### (2).SwicthButtonΪ�����л�״̬��
    ![github](README_Image/2.png)
    ![github](README_Image/3.png)  
    * ʹ�÷�ʽ��
    ```c++
    m_RedSwitchbtn = new SwitchButton(m_pPlot);
    m_RedSwitchbtn->setImage(":/image/btncheckoff7.png",":/image/btncheckon7.png");
    m_RedSwitchbtn->move(510,220);
    m_RedSwitchbtn->setButtonStyle(SwitchButton::ButtonStyle_Image);
    ```
* ### (3). AnimationButton Ϊ������ť��
<br>![](https://github.com/xiedonghuilove/Package/raw/master/QFreamWork/README_Image/animationbutton.gif)
    * ʹ�÷�ʽ��
    ```C++
    AnimationButton *animationbutton = new AnimationButton(this);

    animationbutton->setText("��ҳ");
	animationbutton->setImage(":/image/v-home-ico-home.png");

    connect(animationbutton,SIGNAL(clicked()),this,SLOT(slot_switchbutton()));
    ```
