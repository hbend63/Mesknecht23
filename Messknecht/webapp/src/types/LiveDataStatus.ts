export interface LiveADValue {
    id   : number;
    name : string;
    value: number; 
    unit : string;
    takt : number;    
}
export interface LiveADValueList {
    datavalues: Array<LiveADValue>;
}

export interface Hints {
    time_sync: boolean;
    default_password: boolean;
    radio_problem: boolean;
}