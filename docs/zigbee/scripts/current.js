
/* Copyright (C) 2020 Texas Instruments Incorporated - http://www.ti.com/ 
* 
* 
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions 
* are met:
*
* Redistributions of source code must retain the above copyright 
* notice, this list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the 
* documentation and/or other materials provided with the 
* distribution.
*
* Neither the name of Texas Instruments Incorporated nor the names of
* its contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

/* The following function is used to calulate the tx current based on device and voltage */
function calcTxCurrent(device, TPower, VS){
    var TPower = parseInt(TPower, 10);
    var VS = parseFloat(VS, 10);

    /* TX Current Arrays */
	var CC2652R_Data = [
      [14.362299999999998,13.260379,12.669740333333332,12.163654833333334,11.768801666666667,11.268429,10.931250666666665,10.494000333333334,10.2163515,9.830211666666667,9.547821833333332,9.295419166666667,8.981311833333335,8.739562666666666,8.526001666666668,8.320319,8.126135166666666,7.981850666666666,7.806814  ],
      [13.443502166666669,12.416810833333335,11.889946000000002,11.3952855,11.030032499999999,10.560959666666667,10.237762499999999,9.838760666666666,9.560535166666666,9.207033333333333,9,8.7044955,8.408668166666667,8.183769,7.984712166666665,7.796785833333334,7.620719333333334,7.4569901666666665,7.2955025000000004  ],
      [12.616263833333335,11.612168000000002,11.122665333333336,10.6853585,10.3375985,9.897915333333334,9.614441666666666,9.2239735,8.977504,8.616961333333334,8.392809999999999,8.174362333333333,7.895150666666667,7.682695333333334,7.4949259999999995,7.318193333333333,7.146213166666666,7.0032163333333335,6.852763666666667  ],
      [11.955851000000001,11.01068,10.563633833333334,10.137785666666666,9.8025665,9.396476166666666,9.102328666666667,8.759584166666668,8.508646999999998,8.182065666666668,7.9746615,7.761250166666667,7.4992123333333325,7.299281333333333,7.119235000000001,6.949181333333333,6.788864499999999,6.650589833333332,6.509746833333332  ],
      [11.4225215,10.517094166666666,10.099222833333334,9.6883775,9.371709833333334,8.9883065,8.726882333333334,8.381795666666665,8.160941833333334,7.8518725,7.628628166666668,7.427409333333333,7.179929333333334,6.986410833333334,6.816895,6.650907666666666,6.499069833333333,6.372867333333333,6.236514666666667  ],
      [10.8508385,10.019275166666665,9.589421333333334,9.204275666666666,8.902961833333334,8.539147166666666,8.299818833333333,7.9686388333333324,7.739130499999999,7.463192166666666,7.256281666666666,7.0626256666666665,6.824873166666666,6.647046,6.479920333333333,6.332489166666666,6.177543166666666,6.060412833333333,5.934573166666667  ],
      [9.276318333333334,8.535853166666667,8.194439666666668,7.867591,7.613443833333334,7.298593833333334,7.0865485,6.811559499999999,6.623506333333332,6.380838000000001,6.202828833333334,6.038916499999999,5.837347,5.6816154999999995,5.546,5.413016,5.284068166666667,5.1851986666666665,5.078199333333333  ],
      [8.320723166666667,7.674778833333334,7.3581381666666665,7.058776166666667,6.834334833333333,6.554849,6.366786833333333,6.1144153333333335,5.955854666666667,5.730133500000001,5.576508666666666,5.421692833333334,5.246587833333333,5.1065995,4.976467333333333,4.867526000000001,4.747781833333334,4.6567981666666665,4.562276499999999  ],
      [7.695958,7.077940000000001,6.802753500000001,6.526859833333333,6.321031333333333,6.057658666666666,5.872509833333335,5.6592296666666675,5.509740833333335,5.304409666666667,5.157065833333333,5.017254333333334,4.852172333333334,4.722562,4.609844166666666,4.515361166666667,4.407086166666667,4.313581666666667,4.223474166666667  ],
      [7.2796805,6.689860333333335,6.433578000000001,6.153176833333333,5.986061666666667,5.7376491666666665,5.556458166666666,5.338110999999999,5.1905035,5.006314333333333,4.86777033333333,4.739663333333334,4.5830503333333334,4.4632016666666665,4.3510805,4.2545335,4.163595666666667,4.072617666666667,3.9893238333333336  ]];

    var CC1352P_5_Data=[
      [13.15209895848671,12.14302840318327,11.602158334133836,11.138716783862973,10.777134869958386,10.318924776301946,10.010157879171429,9.609751283178358,9.35549779093071,9.001895004481913,8.7433,8.512165373276849,8.224525459642795,8.003146644053947,7.807580794177996,7.61922943081381,7.441407982150414,7.3092812321051035,7.148993564993035  ],
      [12.242848050937036,11.307851661351853,10.82804171168889,10.377559890566666,10.044928041833332,9.617748624437036,9.323416555833331,8.960050019570367,8.706673148114813,8.384742956296297,8.1962,7.927087335233334,7.6576806697370365,7.452867497533333,7.2715886511592585,7.100446227462963,6.9401044222074075,6.7909980893370365,6.643933065611111  ],
      [11.524758684042256,10.607532924681962,10.160379933665446,9.760907015880258,9.443233722668571,9.041590061739354,8.782641323446299,8.425954791357126,8.200808777608454,7.8714587193403265,7.6667,7.467151490498018,7.2120960222063095,7.018021415004829,6.846497080739347,6.685054567977432,6.5279533892562,6.397328029916878,6.259891884033278  ],
      [10.890406027139838,10.02946388633549,9.622256213048459,9.23435748121305,8.929011351265505,8.559109734584554,8.291175172095603,7.9789743284610495,7.750399412940598,7.452921356306683,7.264,7.069606805337966,6.830920458421129,6.648806297964288,6.484804783250048,6.329905439288343,6.183875231318595,6.057922903603285,5.9296311194316305  ],
      [10.400256815966626,9.575861187132618,9.195387472752035,8.821310962735565,8.532983638628865,8.183893192113679,7.945865321364353,7.631662370895035,7.430573969751966,7.149164962064279,6.9459,6.762689354007707,6.5373577092552395,6.361158251137289,6.206813328167411,6.055681120198783,5.9174320951436234,5.802524155524772,5.678374443845506  ],
      [9.895323376970621,9.13698676623174,8.744985476592811,8.393755392493262,8.118974985542414,7.787197512681541,7.568944218099308,7.266927632377831,7.057629597387918,6.805990146626979,6.6173,6.440697173970416,6.223880946249101,6.061713080661836,5.909304350566142,5.774855840433886,5.6335542464632375,5.526738305410784,5.411979967120443  ],
      [8.71770669696861,8.02183168380459,7.70097780099956,7.393811670223689,7.1549690198044855,6.859079006664513,6.659802854635705,6.4013734475423325,6.2246446752507225,5.9965896130348915,5.8293,5.675258321537862,5.485827157480002,5.339473669846818,5.212024814591986,5.087048992748549,4.96586628320632,4.872950616526497,4.7723946878430965  ],
      [7.889492579242832,7.277025018147556,6.976794600680856,6.6929473640645885,6.48013794854379,6.215136774496779,6.036821135503182,5.797529064780436,5.647185978251865,5.4331630581613615,5.2875,5.140707666717516,4.974677675042915,4.841944390340168,4.718556465676313,4.61526113620912,4.501722842072752,4.415454503537638,4.325831525725833  ],
      [7.377325479343147,6.784895019341588,6.521101950561681,6.256630993639956,6.059324324681396,5.806855966049169,5.629372772193299,5.4249229547971325,5.281623344920458,5.084793418411859,4.94355,4.809527056884253,4.651279877679152,4.527035746605911,4.4189847224414915,4.328413562455331,4.224621426859493,4.134988254448307,4.048611437471147  ],
      [7.038353488164437,6.468086314014684,6.220299937295048,5.949194285131643,5.787619114870599,5.5474416804517865,5.3722573015667505,5.161148511539612,5.018434313779941,4.8403511597609645,4.7064,4.582539845655554,4.431118687152527,4.315243096034868,4.206838832344258,4.1134924397076835,4.025569265545331,3.937607256272228,3.8570746776261937  ]];

    var CC1352P_20_Data= [
      [73.34238199999999,73.51591233333333,74.99093649999999,76.43062216666668,77.80747616666666,79.23968766666665,80.7278065,82.09099233333335,83.52132666666667,84.96301083333334,86.64005266666668,88.29331666666667,89.88325400000001,91.33492666666666,92.90242666666666,94.32589533333334,95.61208766666668,96.84650333333333,97.85215266666667  ],
      [65.62455166666666,65.55711216666667,66.81896533333332,67.89744333333334,68.89847683333333,69.85107083333332,70.956103,71.76021116666665,72.88788949999999,74.11191333333333,75.07156783333333,76.07812666666666,77.12126216666667,77.79042416666667,78.36135566666665,78.76677083333334,79.03352566666666,79.17482466666667,79.23250766666668  ],
      [62.934891333333326,62.70294483333333,63.65925133333334,64.51588449999998,65.30420166666666,66.0039285,66.86198449999999,67.39671266666666,68.2887825,69.24300716666666,69.87202633333332,70.4894205,71.116674,71.36875583333334,71.5181855,71.5796315,71.5710945,71.49451483333334,71.42797499999999  ],
      [61.61147883333334,61.18743699999999,61.94357716666667,62.610089499999994,63.175118166666664,63.670817166666666,64.31694133333333,64.61331466666665,65.22175483333334,65.85894666666668,66.10841599999999,66.32213500000002,66.54826666666668,66.45510233333333,66.37394483333334,66.25778933333333,66.13849800000001,65.99472016666667,66.02652066666667  ],
      [56.545503999999994,55.94236583333333,56.488194,56.9387075,57.29935233333333,57.544296333333335,57.94615816666666,58.13708033333333,58.54727383333333,58.80475366666667,58.77696066666667,58.96243833333333,58.873885,58.69585883333334,58.56436316666667,58.40842416666666,58.25014666666666,58.09592483333333,57.95892416666667  ],
      [52.85674083333333,52.174474499999995,52.607444833333325,52.92278666666666,53.10929066666667,53.33741583333333,53.390977166666666,53.43287949999999,53.55354066666666,53.918232499999995,53.78983766666666,53.93246183333333,53.781565500000006,53.59158333333334,53.43050733333333,53.26742783333333,53.115033833333335,52.95810133333334,52.821825166666656  ],
      [48.510507999999994,47.734384000000006,47.980461,48.14962916666667,48.12052483333334,47.944759999999995,48.067073333333326,48.028183500000004,48.333089166666674,48.452469666666666,48.243599499999995,48.101068,48.19707533333334,47.96765166666666,47.79833300000001,47.62411016666667,47.48166616666666,47.33070033333334,47.19733533333334  ]];


    /* Populate Location Arrays */
    var p_20 = [20,19,18,17,16,15,14];
    var p_5 = [5,4,3,2,1,0,-5,-10,-15,-20];
    var v =[2,2.1,2.2,2.3,2.4,2.5,2.6,2.7,2.8,2.9,3,3.1,3.2, 3.3,3.4,3.5,3.6,3.7,3.8];

    /* Use Look-Up Tables to find the tx current for device and voltage */
	if ((device == "CC13x2P" && TPower >= 14) || (device == "CC26x2P" && TPower >= 14)){
        current=CC1352P_20_Data[p_20.indexOf(TPower)][v.indexOf(Math.round(10*VS)/10)];
	}

	if ((device == "CC13x2P" && TPower < 14) || (device == "CC26x2P" && TPower < 14)){
		current=CC1352P_5_Data[p_5.indexOf(TPower)][v.indexOf(Math.round(10*VS)/10)];
	}

    if ((device == "CC13x2R") || (device == "CC26x2R")){
        current=CC2652R_Data[p_5.indexOf(TPower)][v.indexOf(Math.round(10*VS)/10)];
    }

	return current
}