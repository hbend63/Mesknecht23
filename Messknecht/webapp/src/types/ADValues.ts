export interface ADValue {
    id   : number;
    value: number; 
    takt : number;      
}

export interface ADValueList {
    datavalues: Array<ADValue>;
}