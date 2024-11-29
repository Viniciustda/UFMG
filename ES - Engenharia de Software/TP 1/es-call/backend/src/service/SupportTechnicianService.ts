import SupportTechnician from "../entity/SupportTechnician";
import SupportTechnicianRepository from "../repository/SupportTechnicianRepository";

export default class SupportTechnicianService {

    private repository: SupportTechnicianRepository = new SupportTechnicianRepository();

    async getAll(): Promise<SupportTechnician[]> {
        return this.repository.getAll();
    }
}