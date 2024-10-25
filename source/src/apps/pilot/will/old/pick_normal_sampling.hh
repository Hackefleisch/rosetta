#ifndef INCLUDED_apps_pilot_will_pick_nurmal_sampling_hh
#define INCLUDED_apps_pilot_will_pick_nurmal_sampling_hh

#include <utility/vector1.hh>

int pick_normal_sampling(double tgtcover){
	utility::vector1<int> nsamp;
	utility::vector1<double> cover_rad;
	nsamp.push_back(   72); cover_rad.push_back( 15.144532085111 );
	nsamp.push_back(   92); cover_rad.push_back( 13.676297201544 );
	nsamp.push_back(  122); cover_rad.push_back( 11.685637449452 );
	nsamp.push_back(  132); cover_rad.push_back( 11.216593169250 );
	nsamp.push_back(  162); cover_rad.push_back( 10.193460547112 );
	nsamp.push_back(  192); cover_rad.push_back(  9.246213683185 );
	nsamp.push_back(  212); cover_rad.push_back(  8.838653454591 );
	nsamp.push_back(  252); cover_rad.push_back(  8.127710176122 );
	nsamp.push_back(  272); cover_rad.push_back(  7.760664539139 );
	nsamp.push_back(  282); cover_rad.push_back(  7.617889900096 );
	nsamp.push_back(  312); cover_rad.push_back(  7.272294692156 );
	nsamp.push_back(  362); cover_rad.push_back(  6.758861417535 );
	nsamp.push_back(  372); cover_rad.push_back(  6.622582828824 );
	nsamp.push_back(  392); cover_rad.push_back(  6.458965609790 );
	nsamp.push_back(  432); cover_rad.push_back(  6.170339682766 );
	nsamp.push_back(  482); cover_rad.push_back(  5.814062961918 );
	nsamp.push_back(  492); cover_rad.push_back(  5.784775851139 );
	nsamp.push_back(  492); cover_rad.push_back(  5.752924348142 );
	nsamp.push_back(  522); cover_rad.push_back(  5.595747249333 );
	nsamp.push_back(  572); cover_rad.push_back(  5.355354417598 );
	nsamp.push_back(  612); cover_rad.push_back(  5.154687189818 );
	nsamp.push_back(  632); cover_rad.push_back(  5.074888764183 );
	nsamp.push_back(  642); cover_rad.push_back(  5.056115072666 );
	nsamp.push_back(  672); cover_rad.push_back(  4.929797462352 );
	// nsamp.push_back(  732); cover_rad.push_back(  4.729078589132 );
	// nsamp.push_back(  752); cover_rad.push_back(  4.648295848279 );
	// nsamp.push_back(  762); cover_rad.push_back(  4.617219818603 );
	// nsamp.push_back(  792); cover_rad.push_back(  4.533029350096 );
	// nsamp.push_back(  812); cover_rad.push_back(  4.490479603994 );
	// nsamp.push_back(  842); cover_rad.push_back(  4.402124515864 );
	// nsamp.push_back(  912); cover_rad.push_back(  4.233169088336 );
	// nsamp.push_back(  912); cover_rad.push_back(  4.218557913621 );
	// nsamp.push_back(  932); cover_rad.push_back(  4.174239460090 );
	// nsamp.push_back(  972); cover_rad.push_back(  4.091374894291 );
	// nsamp.push_back( 1002); cover_rad.push_back(  4.038657206107 );
	nsamp.push_back( 1032); cover_rad.push_back(  3.974509870333 );
	// nsamp.push_back( 1082); cover_rad.push_back(  3.871891979419 );
	// nsamp.push_back( 1092); cover_rad.push_back(  3.853939041670 );
	// nsamp.push_back( 1112); cover_rad.push_back(  3.830953328855 );
	// nsamp.push_back( 1122); cover_rad.push_back(  3.804178860903 );
	// nsamp.push_back( 1172); cover_rad.push_back(  3.725307578617 );
	// nsamp.push_back( 1212); cover_rad.push_back(  3.669436148858 );
	// nsamp.push_back( 1242); cover_rad.push_back(  3.621422198143 );
	// nsamp.push_back( 1272); cover_rad.push_back(  3.569725534206 );
	// nsamp.push_back( 1292); cover_rad.push_back(  3.542654967391 );
	// nsamp.push_back( 1332); cover_rad.push_back(  3.498272262557 );
	// nsamp.push_back( 1332); cover_rad.push_back(  3.491241689736 );
	// nsamp.push_back( 1392); cover_rad.push_back(  3.417577813358 );
	// nsamp.push_back( 1442); cover_rad.push_back(  3.362062489397 );
	// nsamp.push_back( 1472); cover_rad.push_back(  3.325210509179 );
	// nsamp.push_back( 1472); cover_rad.push_back(  3.317695364929 );
	// nsamp.push_back( 1482); cover_rad.push_back(  3.306416210909 );
	// nsamp.push_back( 1512); cover_rad.push_back(  3.274691399950 );
	// nsamp.push_back( 1562); cover_rad.push_back(  3.223730891645 );
	// nsamp.push_back( 1572); cover_rad.push_back(  3.218586239818 );
	// nsamp.push_back( 1632); cover_rad.push_back(  3.155616147025 );
	// nsamp.push_back( 1692); cover_rad.push_back(  3.102197973127 );
	// nsamp.push_back( 1692); cover_rad.push_back(  3.093711926789 );
	// nsamp.push_back( 1712); cover_rad.push_back(  3.076018937021 );
	// nsamp.push_back( 1722); cover_rad.push_back(  3.073309760621 );
	// nsamp.push_back( 1752); cover_rad.push_back(  3.042047458937 );
	nsamp.push_back( 1812); cover_rad.push_back(  2.992780255006 );
	// nsamp.push_back( 1832); cover_rad.push_back(  2.980200374028 );
	// nsamp.push_back( 1892); cover_rad.push_back(  2.930147770804 );
	// nsamp.push_back( 1922); cover_rad.push_back(  2.902260274250 );
	// nsamp.push_back( 1932); cover_rad.push_back(  2.894701570066 );
	// nsamp.push_back( 1962); cover_rad.push_back(  2.879618518442 );
	// nsamp.push_back( 1962); cover_rad.push_back(  2.873285981385 );
	// nsamp.push_back( 1992); cover_rad.push_back(  2.856557536608 );
	// nsamp.push_back( 2012); cover_rad.push_back(  2.838572507835 );
	// nsamp.push_back( 2082); cover_rad.push_back(  2.791655507280 );
	// nsamp.push_back( 2112); cover_rad.push_back(  2.774616460782 );
	// nsamp.push_back( 2172); cover_rad.push_back(  2.734196931924 );
	// nsamp.push_back( 2172); cover_rad.push_back(  2.729614889549 );
	// nsamp.push_back( 2192); cover_rad.push_back(  2.717406611262 );
	// nsamp.push_back( 2232); cover_rad.push_back(  2.693850201666 );
	// nsamp.push_back( 2252); cover_rad.push_back(  2.686837347164 );
	// nsamp.push_back( 2282); cover_rad.push_back(  2.668138043779 );
	// nsamp.push_back( 2292); cover_rad.push_back(  2.659385677717 );
	// nsamp.push_back( 2372); cover_rad.push_back(  2.615107796463 );
	// nsamp.push_back( 2412); cover_rad.push_back(  2.595513341479 );
	// nsamp.push_back( 2432); cover_rad.push_back(  2.579274217401 );
	// nsamp.push_back( 2442); cover_rad.push_back(  2.573968426865 );
	// nsamp.push_back( 2472); cover_rad.push_back(  2.562415005674 );
	// nsamp.push_back( 2472); cover_rad.push_back(  2.558841875674 );
	// nsamp.push_back( 2522); cover_rad.push_back(  2.534187181036 );
	// nsamp.push_back( 2562); cover_rad.push_back(  2.518246302211 );
	// nsamp.push_back( 2592); cover_rad.push_back(  2.502876199535 );
	// nsamp.push_back( 2592); cover_rad.push_back(  2.500582676677 );
	// nsamp.push_back( 2682); cover_rad.push_back(  2.459012620380 );
	// nsamp.push_back( 2712); cover_rad.push_back(  2.442145526900 );
	// nsamp.push_back( 2732); cover_rad.push_back(  2.438092954390 );
	// nsamp.push_back( 2732); cover_rad.push_back(  2.433380291879 );
	// nsamp.push_back( 2772); cover_rad.push_back(  2.416391788693 );
	// nsamp.push_back( 2792); cover_rad.push_back(  2.410654819470 );
	// nsamp.push_back( 2832); cover_rad.push_back(  2.391387928112 );
	// nsamp.push_back( 2892); cover_rad.push_back(  2.369561413480 );
	// nsamp.push_back( 2912); cover_rad.push_back(  2.359001520995 );
	// nsamp.push_back( 2922); cover_rad.push_back(  2.356776129106 );
	// nsamp.push_back( 3002); cover_rad.push_back(  2.320969691511 );
	// nsamp.push_back( 3012); cover_rad.push_back(  2.320099127117 );
	// nsamp.push_back( 3012); cover_rad.push_back(  2.317097689455 );
	// nsamp.push_back( 3042); cover_rad.push_back(  2.306023037552 );
	// nsamp.push_back( 3072); cover_rad.push_back(  2.298648556024 );
	// nsamp.push_back( 3092); cover_rad.push_back(  2.287905990245 );
	// nsamp.push_back( 3132); cover_rad.push_back(  2.275654652283 );
	// nsamp.push_back( 3162); cover_rad.push_back(  2.263060249423 );
	// nsamp.push_back( 3242); cover_rad.push_back(  2.237453828613 );
	// nsamp.push_back( 3252); cover_rad.push_back(  2.232090438109 );
	// nsamp.push_back( 3272); cover_rad.push_back(  2.226705320548 );
	// nsamp.push_back( 3312); cover_rad.push_back(  2.209428219014 );
	// nsamp.push_back( 3332); cover_rad.push_back(  2.202914984094 );
	// nsamp.push_back( 3362); cover_rad.push_back(  2.195734837706 );
	// nsamp.push_back( 3372); cover_rad.push_back(  2.190267930059 );
	// nsamp.push_back( 3432); cover_rad.push_back(  2.174271529630 );
	// nsamp.push_back( 3432); cover_rad.push_back(  2.171579881476 );
	// nsamp.push_back( 3492); cover_rad.push_back(  2.154813888078 );
	// nsamp.push_back( 3512); cover_rad.push_back(  2.147226270662 );
	// nsamp.push_back( 3612); cover_rad.push_back(  2.119297750869 );
	// nsamp.push_back( 3612); cover_rad.push_back(  2.117750682729 );
	// nsamp.push_back( 3642); cover_rad.push_back(  2.110175634237 );
	// nsamp.push_back( 3632); cover_rad.push_back(  2.109686870986 );
	// nsamp.push_back( 3642); cover_rad.push_back(  2.106778060218 );
	// nsamp.push_back( 3672); cover_rad.push_back(  2.098426491729 );
	// nsamp.push_back( 3722); cover_rad.push_back(  2.084735990702 );
	// nsamp.push_back( 3732); cover_rad.push_back(  2.083793336434 );
	// nsamp.push_back( 3792); cover_rad.push_back(  2.065873127601 );
	// nsamp.push_back( 3812); cover_rad.push_back(  2.062647863476 );
	// nsamp.push_back( 3872); cover_rad.push_back(  2.046038776330 );
	// nsamp.push_back( 3882); cover_rad.push_back(  2.042217702056 );
	// nsamp.push_back( 3972); cover_rad.push_back(  2.017185890981 );
	// nsamp.push_back( 3992); cover_rad.push_back(  2.014256907819 );
	// nsamp.push_back( 4002); cover_rad.push_back(  2.012994074021 );
	// nsamp.push_back( 3992); cover_rad.push_back(  2.012218084622 );
	// nsamp.push_back( 4032); cover_rad.push_back(  2.005186402927 );
	// nsamp.push_back( 4032); cover_rad.push_back(  2.002553214516 );
	// nsamp.push_back( 4092); cover_rad.push_back(  1.988228662936 );
	// nsamp.push_back( 4122); cover_rad.push_back(  1.982533942866 );
	// nsamp.push_back( 4172); cover_rad.push_back(  1.969474459217 );
	// nsamp.push_back( 4212); cover_rad.push_back(  1.961913799540 );
	// nsamp.push_back( 4272); cover_rad.push_back(  1.947624805253 );
	// nsamp.push_back( 4272); cover_rad.push_back(  1.946647942289 );
	// nsamp.push_back( 4322); cover_rad.push_back(  1.933658161783 );
	// nsamp.push_back( 4332); cover_rad.push_back(  1.931415123400 );
	// nsamp.push_back( 4362); cover_rad.push_back(  1.924961862026 );
	// nsamp.push_back( 4392); cover_rad.push_back(  1.920173984113 );
	// nsamp.push_back( 4412); cover_rad.push_back(  1.916844661950 );
	// nsamp.push_back( 4412); cover_rad.push_back(  1.914368254712 );
	// nsamp.push_back( 4442); cover_rad.push_back(  1.910110821950 );
	// nsamp.push_back( 4482); cover_rad.push_back(  1.899723818959 );
	// nsamp.push_back( 4532); cover_rad.push_back(  1.890519597739 );
	// nsamp.push_back( 4572); cover_rad.push_back(  1.881274549723 );
	// nsamp.push_back( 4632); cover_rad.push_back(  1.870551478387 );
	// nsamp.push_back( 4682); cover_rad.push_back(  1.859343469967 );
	// nsamp.push_back( 4692); cover_rad.push_back(  1.858170711336 );
	// nsamp.push_back( 4692); cover_rad.push_back(  1.855708148791 );
	// nsamp.push_back( 4712); cover_rad.push_back(  1.851830407685 );
	// nsamp.push_back( 4752); cover_rad.push_back(  1.844278308492 );
	// nsamp.push_back( 4812); cover_rad.push_back(  1.834304363912 );
	// nsamp.push_back( 4812); cover_rad.push_back(  1.833066426224 );
	// nsamp.push_back( 4842); cover_rad.push_back(  1.829461011271 );
	// nsamp.push_back( 4872); cover_rad.push_back(  1.823612933411 );
	nsamp.push_back( 4892); cover_rad.push_back(  1.818333020904 );
	// nsamp.push_back( 4962); cover_rad.push_back(  1.806557632344 );
	// nsamp.push_back( 4992); cover_rad.push_back(  1.800316761495 );
	// nsamp.push_back( 5072); cover_rad.push_back(  1.787312335561 );
	// nsamp.push_back( 5072); cover_rad.push_back(  1.784740014606 );
	// nsamp.push_back( 5082); cover_rad.push_back(  1.782975472798 );
	// nsamp.push_back( 5112); cover_rad.push_back(  1.779310310330 );
	// nsamp.push_back( 5112); cover_rad.push_back(  1.777885600118 );
	// nsamp.push_back( 5132); cover_rad.push_back(  1.776515099717 );
	// nsamp.push_back( 5162); cover_rad.push_back(  1.769523657985 );
	// nsamp.push_back( 5232); cover_rad.push_back(  1.757933964293 );
	// nsamp.push_back( 5252); cover_rad.push_back(  1.755639722914 );
	// nsamp.push_back( 5292); cover_rad.push_back(  1.749696685504 );
	// nsamp.push_back( 5322); cover_rad.push_back(  1.744585656368 );
	// nsamp.push_back( 5322); cover_rad.push_back(  1.743279127099 );
	// nsamp.push_back( 5412); cover_rad.push_back(  1.729648442054 );
	// nsamp.push_back( 5432); cover_rad.push_back(  1.725781837879 );
	// nsamp.push_back( 5472); cover_rad.push_back(  1.718159484815 );
	// nsamp.push_back( 5492); cover_rad.push_back(  1.715075232571 );
	// nsamp.push_back( 5532); cover_rad.push_back(  1.711160016297 );
	// nsamp.push_back( 5532); cover_rad.push_back(  1.709063208448 );
	// nsamp.push_back( 5562); cover_rad.push_back(  1.705703050336 );
	// nsamp.push_back( 5592); cover_rad.push_back(  1.701687873242 );
	// nsamp.push_back( 5592); cover_rad.push_back(  1.700124308904 );
	// nsamp.push_back( 5672); cover_rad.push_back(  1.688345875312 );
	// nsamp.push_back( 5712); cover_rad.push_back(  1.683326439481 );
	// nsamp.push_back( 5762); cover_rad.push_back(  1.676596846619 );
	// nsamp.push_back( 5772); cover_rad.push_back(  1.673891352565 );
	// nsamp.push_back( 5792); cover_rad.push_back(  1.672104094657 );
	// nsamp.push_back( 5882); cover_rad.push_back(  1.658949422434 );
	// nsamp.push_back( 5882); cover_rad.push_back(  1.657117551926 );
	// nsamp.push_back( 5892); cover_rad.push_back(  1.656964997043 );
	// nsamp.push_back( 5892); cover_rad.push_back(  1.655703736863 );
	// nsamp.push_back( 5922); cover_rad.push_back(  1.651617711575 );
	// nsamp.push_back( 5972); cover_rad.push_back(  1.644902119021 );
	// nsamp.push_back( 6012); cover_rad.push_back(  1.641227096458 );
	// nsamp.push_back( 6032); cover_rad.push_back(  1.637797846484 );
	// nsamp.push_back( 6042); cover_rad.push_back(  1.635577744593 );
	// nsamp.push_back( 6072); cover_rad.push_back(  1.632872144303 );
	// nsamp.push_back( 6132); cover_rad.push_back(  1.623753111604 );
	// nsamp.push_back( 6192); cover_rad.push_back(  1.616639079196 );
	// nsamp.push_back( 6242); cover_rad.push_back(  1.609583562972 );
	// nsamp.push_back( 6252); cover_rad.push_back(  1.609359748756 );
	// nsamp.push_back( 6282); cover_rad.push_back(  1.605389514175 );
	// nsamp.push_back( 6312); cover_rad.push_back(  1.599589033694 );
	// nsamp.push_back( 6332); cover_rad.push_back(  1.597095309698 );
	// nsamp.push_back( 6372); cover_rad.push_back(  1.593745581155 );
	// nsamp.push_back( 6372); cover_rad.push_back(  1.593254747857 );
	// nsamp.push_back( 6372); cover_rad.push_back(  1.592230474801 );
	// nsamp.push_back( 6432); cover_rad.push_back(  1.584992376426 );
	// nsamp.push_back( 6512); cover_rad.push_back(  1.576782082446 );
	// nsamp.push_back( 6512); cover_rad.push_back(  1.575433582261 );
	// nsamp.push_back( 6522); cover_rad.push_back(  1.574973101230 );
	// nsamp.push_back( 6572); cover_rad.push_back(  1.569375530683 );
	// nsamp.push_back( 6612); cover_rad.push_back(  1.563667917592 );
	// nsamp.push_back( 6692); cover_rad.push_back(  1.554955846809 );
	// nsamp.push_back( 6732); cover_rad.push_back(  1.549841115865 );
	// nsamp.push_back( 6762); cover_rad.push_back(  1.547307332801 );
	// nsamp.push_back( 6752); cover_rad.push_back(  1.546527850635 );
	// nsamp.push_back( 6762); cover_rad.push_back(  1.545377924235 );
	// nsamp.push_back( 6792); cover_rad.push_back(  1.543781689508 );
	// nsamp.push_back( 6792); cover_rad.push_back(  1.542048496457 );
	// nsamp.push_back( 6842); cover_rad.push_back(  1.536574621638 );
	// nsamp.push_back( 6872); cover_rad.push_back(  1.534121761809 );
	// nsamp.push_back( 6882); cover_rad.push_back(  1.533426267038 );
	// nsamp.push_back( 6912); cover_rad.push_back(  1.528963079847 );
	// nsamp.push_back( 7002); cover_rad.push_back(  1.519288003357 );
	// nsamp.push_back( 7682); cover_rad.push_back(  1.449774552065 );
	nsamp.push_back( 8092); cover_rad.push_back(  1.405300351294 );
	// nsamp.push_back( 8192); cover_rad.push_back(  1.403881943807 );
	// nsamp.push_back( 8672); cover_rad.push_back(  1.364413944909 );
	// nsamp.push_back( 9722); cover_rad.push_back(  1.288545767384 );
	// nsamp.push_back(10832); cover_rad.push_back(  1.220670083820 );
	// nsamp.push_back(12002); cover_rad.push_back(  1.159587155975 );
	// nsamp.push_back(13232); cover_rad.push_back(  1.104325930573 );
	// nsamp.push_back(14522); cover_rad.push_back(  1.054091991326 );
	// nsamp.push_back(15872); cover_rad.push_back(  1.008229191699 );
	nsamp.push_back(17282); cover_rad.push_back(  0.966190795787 );
	// nsamp.push_back(18752); cover_rad.push_back(  0.927517598993 );
	// nsamp.push_back(20282); cover_rad.push_back(  0.891821076004 );
	// nsamp.push_back(21872); cover_rad.push_back(  0.858770306968 );
	// nsamp.push_back(23522); cover_rad.push_back(  0.828081659327 );
	// nsamp.push_back(25232); cover_rad.push_back(  0.799510654132 );
	// nsamp.push_back(27002); cover_rad.push_back(  0.772845388796 );
	// nsamp.push_back(28832); cover_rad.push_back(  0.747901370396 );
	// nsamp.push_back(30722); cover_rad.push_back(  0.724517158473 );
	// nsamp.push_back(32672); cover_rad.push_back(  0.702550866516 );
	// nsamp.push_back(34682); cover_rad.push_back(  0.681877319450 );
	// nsamp.push_back(36752); cover_rad.push_back(  0.662385678203 );
	// nsamp.push_back(38882); cover_rad.push_back(  0.643977395866 );
	// nsamp.push_back(41072); cover_rad.push_back(  0.626564612737 );
	// nsamp.push_back(43322); cover_rad.push_back(  0.610068684751 );
	// nsamp.push_back(45632); cover_rad.push_back(  0.594419060597 );
	// nsamp.push_back(48002); cover_rad.push_back(  0.579552240743 );
	// nsamp.push_back(50432); cover_rad.push_back(  0.565410925600 );
	// nsamp.push_back(52922); cover_rad.push_back(  0.551943280335 );
	// nsamp.push_back(55472); cover_rad.push_back(  0.539102268802 );
	// nsamp.push_back(58082); cover_rad.push_back(  0.526845169006 );
	// nsamp.push_back(60752); cover_rad.push_back(  0.515133033200 );
	// nsamp.push_back(63482); cover_rad.push_back(  0.503930303949 );
	// nsamp.push_back(66272); cover_rad.push_back(  0.493204458195 );
	// nsamp.push_back(69122); cover_rad.push_back(  0.482936940333 );
	// nsamp.push_back(72032); cover_rad.push_back(  0.473076997075 );
	// nsamp.push_back(75002); cover_rad.push_back(  0.463611644179 );
	// nsamp.push_back(78032); cover_rad.push_back(  0.454517612602 );
	if(tgtcover > cover_rad.front()) return nsamp.front();
	for(int i = 2; i <= (int)nsamp.size(); ++i){
		if(cover_rad[i] <= tgtcover && tgtcover < cover_rad[i-1]) return nsamp[i];
	}
	if(tgtcover <= cover_rad.back()) return nsamp.back();
 	utility_exit_with_message("sampling level unsupported, 1.0 - 5.0 degrees ( currently 1.0, 1.4, 2.0, 3.0, 4.0, 5.0 )");
}


#endif


