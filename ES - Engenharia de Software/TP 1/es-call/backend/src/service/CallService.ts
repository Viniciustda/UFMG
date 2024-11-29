import Call from "../entity/Call";
import CallConstantes from "../entity/CallConstantes";
import SupportTechnician from "../entity/SupportTechnician";
import CallRepository from "../repository/CallRepository";
import SupportTechnicianRepository from "../repository/SupportTechnicianRepository";

export default class CallService {

    private callRepository: CallRepository = new CallRepository();
    private supportTechnicianRepository: SupportTechnicianRepository = new SupportTechnicianRepository();

    async getAll(): Promise<Call[]> {
        const supportTechnicians = await this.supportTechnicianRepository.getAll();
        const calls = await this.callRepository.getAll();

        calls.forEach(call => {
            const stId = call.getSupportTechnicianId();
            if (stId !== null) {
                const stName = supportTechnicians.filter(supportTechnician => supportTechnician.getId() === stId)[0].getName();
                call.setSupportTechnicianName(stName);
            }
        });
        return calls;
    }

    async addSupportTechnician(callId: number, supportTechnicianId: number): Promise<void> {
        const technicianAddedDate = new Date();
        this.callRepository.addSupportTechnician(
            callId,
            supportTechnicianId,
            technicianAddedDate,
            CallConstantes.ALLOCATED_STATUS
        );
    }

    async addLowPriority(callId: number): Promise<void> {
        this.callRepository.setPriority(callId, CallConstantes.LOW_PRIORITY);
    }

    async addMediumPriority(callId: number): Promise<void> {
        this.callRepository.setPriority(callId, CallConstantes.MEDIUM_PRIORITY);
    }

    async addHighPriority(callId: number): Promise<void> {
        this.callRepository.setPriority(callId, CallConstantes.HIGH_PRIORITY);
    }

    async createNewCall(
        name: string,
        phoneNumber: string,
        description: string,
    ): Promise<void> {
        const newCall = new Call(
            null,
            name,
            phoneNumber,
            description,
            CallConstantes.WAITING_STATUS, CallConstantes.LOW_PRIORITY, new Date());
        
        this.callRepository.create(newCall);
    }

    async getAllBySupportTechnician(supportTechnicianId: number): Promise<Call[]> {
        const supportTechnicians = await this.supportTechnicianRepository.getAll();
        const calls = await this.callRepository.getAllBySupportTechnician(supportTechnicianId);

        calls.forEach(call => {
            const stId = call.getSupportTechnicianId();
            if (stId !== null) {
                const stName = supportTechnicians.filter(supportTechnician => supportTechnician.getId() === stId)[0].getName();
                call.setSupportTechnicianName(stName);
            }
        });
        return calls;
    }

    async finishCall(callId: number): Promise<void> {
        this.callRepository.setStatus(callId, CallConstantes.FINALIZED_STATUS);
    }

    async cancelCall(callId: number): Promise<void> {
        this.callRepository.setStatus(callId, CallConstantes.CANCELLED_STATUS);
    }
}
