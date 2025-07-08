/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/

#pragma once

#include <QDialog>
#include <QRadioButton>
#include <QStackedWidget>
#include <xflgeom/geom3d/vector3d.h>
#include <xfl3d/testgl/gl3dsurface.h>


namespace xfl
{
    enum enumObjectiveType {MINIMIZE, EQUALIZE, MAXIMIZE}; // defines whether the objective is to minimize, maximize or make equal
}


struct OptObjective
{
        OptObjective() : m_Index{-1}, m_bActive{true}, m_Target{0.0}, m_MaxError{0.0}, m_Type{xfl::EQUALIZE}
    {}

    OptObjective(QString const &name, int index, bool bActive, double target, double maxerror, xfl::enumObjectiveType type) :
        m_Name{name}, m_Index{index}, m_bActive{bActive}, m_Target{target}, m_MaxError{maxerror}, m_Type{type}
    {}

    QString m_Name;
    int m_Index{-1};         /**< the objective's index in the array of possible objectives */
    bool m_bActive{true};    /**< true if this objective is active in the current optimization task */
    double m_Target{0.0};    /**< this objective's target value */
    double m_MaxError{0.0};  /**< this objective's maximum error */
    xfl::enumObjectiveType m_Type = xfl::EQUALIZE;
};


struct OptVariable
{
    OptVariable() : m_Min{0.0}, m_Max{0.0}
    {}

    OptVariable(QString const &name, double valmin, double valmax) : m_Name{name}, m_Min{valmin}, m_Max{valmax}
    {}

    OptVariable(QString const &name, double val) : m_Name{name}, m_Min{val}, m_Max{val}
    {}

    QString m_Name;
    double m_Min{0.0};
    double m_Max{0.0};
};


struct OptCp
{
    OptCp() : m_iMin{-1}, m_iMax{-1}
    {}

    OptCp(int iMin, int iMax, QVector<double> Cp) : m_iMin{iMin}, m_iMax{iMax}, m_Cp{Cp}
    {}

    bool isActive() const {return m_iMin>=0 && m_iMax>=0;}

    int m_iMin{-1};           /**< the index on the curve of the spline's first control point */
    int m_iMax{-1};           /**< the index on the curve of the spline's last control point */
    QVector<double>m_Cp;  /**< the array of objective Cp values from node m_iMin to node m_iMax */
};


/**
 * @class Multi-Objective Particle
 * To use in single objective PSO or GA, set NObjectives=1 and NBest=1
 */
class Particle
{
    public:
        Particle();

        int dimension() const {return m_Position.size();}
        int nObjectives() const {return m_Fitness.size();}
        int nBest() const {return m_BestPosition.size();}

        void resetBestError();
        double error(int iobj) const {return m_Error.at(iobj);}
        void setError(int iobj, double err) {m_Error[iobj]=err;}

        double bestError(int iFront, int iobj) const {return m_BestError.at(iFront).at(iobj);}
        void setBestError(int iFront, int iobj, double err) {m_BestError[iFront][iobj]=err;}
        void setBestPosition(int iFront, int idim, double pos) {m_BestPosition[iFront][idim]=pos;}
        void storeBestPosition(int ifront) {m_BestPosition[ifront]=m_Position;}

        bool isSame(const Particle &p) const;

        QVector<double> const &velocity() const {return m_Velocity;}
        QVector<double> const &position() const {return m_Position;}

        void setPos(int i, double dble) {m_Position[i]=dble;}
        void setVel(int i, double dble) {m_Velocity[i]=dble;}

        double pos(int i) const {return m_Position.at(i);}
        double bestPos(int iFront, int iComponent) const {return m_BestPosition.at(iFront).at(iComponent);}
        double vel(int i) const {return m_Velocity.at(i);}

        void setFitness(int i, double f) {m_Fitness[i]=f;}
        double fitness(int i) const {return m_Fitness.at(i);}

        void initializeBest();
        void updateBest();

        void resizeArrays(int dim, int nobj, int nbest);

        bool dominates(Particle const* pOther) const;

        QVector<double> &Cp(int iobj) {return m_OptCp[iobj].m_Cp;}


        bool isConverged()  const {return m_bIsConverged;}
        void setConverged(bool b) {m_bIsConverged=b;}

        void setInParetoFront(bool b) {m_bIsInParetoFront = b;}
        bool isInParetoFront() const {return m_bIsInParetoFront;}

    private:
        // size = dimension = nVariables
        QVector<double> m_Position;
        QVector<double> m_Velocity;

        //size = NObjectives
        QVector<double> m_Fitness; /** the value of each objective function for this particle */
        QVector<double> m_Error;   /** the error associated to each objective */

        //size = PARETOSIZE
        QVector<QVector<double>> m_BestError;    /** the particle's personal best errors achieved so far; size=nObjectives*/
        QVector<QVector<double>> m_BestPosition; /** the particle's personal best positions achieved so far; size=dimension */

        bool m_bIsInParetoFront;

        //XFoil specific
        bool m_bIsConverged;

    public:
        /** @todo the following fields do not belong here, move to a subclass*/
        QVector<OptCp> m_OptCp; /** Only used in the case of Cp optimization*/
        double m_CL=0, m_CDi=0;

};

class IntEdit;
class FloatEdit;
class PlainTextOutput;

class gl3dOptim2d : public gl3dSurface
{
    Q_OBJECT
    public:
        gl3dOptim2d();
        ~gl3dOptim2d();

        void glRenderView() override;
        void glMake3dObjects() override;

        static void loadSettings(QSettings &settings);
        static void saveSettings(QSettings &settings);

    private:
        void setupLayout();

        double PSO_error(double z) const;
        void resetParticles();
        void readData();

        void bound(double &val) const {val = std::min(m_HalfSide, val);val = std::max(-m_HalfSide, val);}

        //PSO specific
        void moveSwarm();

        //GA specific
        double GA_error(double z) const;
        int leastFit();
        int popSize() const {return m_Swarm.size();}
        void calculateFitness();
        void GA_crossOver();
        void evaluatePopulation();
        void listGAPopulation(QString &log) const;
        void makeNewGen();
        void makeSelection();
        void mutateGaussian();
        void mutatePopulation();
        void selection();

        //Simplex specific
        void moveSimplex();

    private slots:
        void onAlgorithm();
        void onMakeSurface();
        void onTarget();
        void onIteration();

        //PSO specific
        void onMakeSwarm();
        void onResetPSODefaults();
        void onSwarm();

        //GA specific
        void onMakeGAPopulation();
        void onResetGADefaults();
        void onStartGA();

        //Simplex specific
        void onMakeSimplex();
        void onStartSimplex();

    private:       
        //common
        double m_Error;
        double m_BestError;
        int m_Iter;
        int m_iBest;
        Vector2d m_BestPosition; /**< best solution found by any particle in the swarm */
        QVector<Particle> m_Swarm; /**< the swarm or the population in the case of the GA*/

        //Simplex specific
        Vector3d m_S[3];
        bool m_bglResetTriangle;

        //Common
        QRadioButton *m_prbPSO, *m_prbGA, *m_prbSimplex;
        QStackedWidget *m_pswAlgo;
        PlainTextOutput *m_ppt;
        IntEdit *m_piePopSize;
        IntEdit *m_pieUpdateDt;
        FloatEdit *m_pdeMaxError;
        QTimer m_Timer;
        QRadioButton *m_prbMin, *m_prbMax;

        //PSO specific
        FloatEdit *m_pdeInertiaWeight;
        FloatEdit *m_pdeCognitiveWeight;
        FloatEdit *m_pdeSocialWeight;
        FloatEdit *m_pdePropRegenerate;
        QPushButton *m_ppbSwarm;

        // GA specific
        FloatEdit *m_pdeProbXOver, *m_pdeProbMutation, *m_pdeSigmaMutation;
        QPushButton *m_ppbStartGA;

        // Simplex specific
        QPushButton *m_ppbNewSimplex, *m_ppbSimplex;

        QOpenGLBuffer m_vboTriangle;

        static int s_iAlgo;
        static bool s_bMinimum;
        static int s_PopSize;
        static int s_Dt;
        static double s_MaxError;
        static double s_InertiaWeight;
        static double s_CognitiveWeight;
        static double s_SocialWeight;
        static double s_ProbRegenerate;

        static int s_MaxIter;
        static double s_ProbXOver;       /** probability of crossover */
        static double s_ProbMutation;    /** probability of mutation */
        static double s_SigmaMutation;   /** standard deviation of the gaussian mutation */
};

