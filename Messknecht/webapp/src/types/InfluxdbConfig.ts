export interface InfluxDb {    
        url: string;
        token: string;
        org: string;
        bucket: string;  
        name: string;   
}

export interface InfluxDbConfig {  
    influxdb: InfluxDb; 
}