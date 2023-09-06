export interface ADChannel {    
    name: string;
    unit: string;
    id  : number;    
    dmin : number;
    dmax : number;
    smin : number;
    smax : number;
    takt : number;    
}

export interface ADC {
    typename: string;
    pollintervall: number;      
    channels: Array<ADChannel>;
}

export interface ADConfig {  
    adc: ADC; 
}